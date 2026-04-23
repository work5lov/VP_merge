#include "backend/pegeneratorservice.h"

#include <QChar>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFont>

namespace {
constexpr int kPeColumns[2][4] = {
    {4, 7, 12, 15},
    {4, 7, 11, 13}
};

QString cellNameFromPosition(int row, int column)
{
    return QString("%1%2").arg(QChar('A' + column - 1)).arg(row);
}
}

bool PeGeneratorService::generate(const ProjectData &project,
                                  const QString &outputPath,
                                  QString *errorMessage) const
{
    if (project.peLines.isEmpty()) {
        setError(errorMessage, QStringLiteral("Нет данных ПЭ для генерации"));
        return false;
    }

    if (outputPath.trimmed().isEmpty()) {
        setError(errorMessage, QStringLiteral("Не указан путь выходного файла ПЭ"));
        return false;
    }

    if (!ensureTemplateCopied(outputPath, errorMessage)) {
        return false;
    }

    return populateDocument(project, outputPath, errorMessage);
}

bool PeGeneratorService::ensureTemplateCopied(const QString &outputPath, QString *errorMessage) const
{
    const QString sourceFilePath = QStringLiteral("perechen.xlsx");
    QFile sourceFile(sourceFilePath);
    if (!sourceFile.exists()) {
        setError(errorMessage, QStringLiteral("Не найден шаблон ПЭ: %1").arg(sourceFilePath));
        return false;
    }

    QFileInfo destinationInfo(outputPath);
    QDir destinationDir = destinationInfo.dir();
    if (!destinationDir.exists() && !destinationDir.mkpath(QStringLiteral("."))) {
        setError(errorMessage, QStringLiteral("Не удалось создать каталог для ПЭ: %1").arg(destinationDir.absolutePath()));
        return false;
    }

    if (destinationInfo.exists() && !QFile::remove(outputPath)) {
        setError(errorMessage, QStringLiteral("Не удалось удалить существующий файл ПЭ: %1").arg(outputPath));
        return false;
    }

    if (!sourceFile.copy(outputPath)) {
        setError(errorMessage, QStringLiteral("Не удалось скопировать шаблон ПЭ в %1").arg(outputPath));
        return false;
    }

    return true;
}

bool PeGeneratorService::cloneSheetTemplate(const QString &filePath,
                                            const QString &sheetName,
                                            QString *errorMessage) const
{
    TemplateSnapshot snapshot;
    if (!captureTemplateSnapshot(&snapshot, errorMessage)) {
        return false;
    }

    QXlsx::Document xlsx(filePath);
    xlsx.addSheet(sheetName);
    if (!xlsx.selectSheet(sheetName)) {
        setError(errorMessage, QStringLiteral("Не удалось выбрать лист %1 в файле ПЭ").arg(sheetName));
        return false;
    }

    for (auto it = snapshot.columnWidths.cbegin(); it != snapshot.columnWidths.cend(); ++it) {
        xlsx.setColumnWidth(it.key(), it.key(), it.value());
    }
    for (auto it = snapshot.rowHeights.cbegin(); it != snapshot.rowHeights.cend(); ++it) {
        xlsx.setRowHeight(it.key(), it.value());
    }
    for (const QXlsx::CellRange &range : snapshot.mergedCells) {
        xlsx.mergeCells(range, snapshot.cellFormats.value(range.toString()));
    }
    for (auto it = snapshot.cellsWithText.cbegin(); it != snapshot.cellsWithText.cend(); ++it) {
        xlsx.write(it.key(), it.value(), snapshot.cellFormats.value(it.key()));
    }

    if (!xlsx.save()) {
        setError(errorMessage, QStringLiteral("Не удалось сохранить лист %1 в файле ПЭ").arg(sheetName));
        return false;
    }

    return true;
}

bool PeGeneratorService::captureTemplateSnapshot(TemplateSnapshot *snapshot,
                                                 QString *errorMessage) const
{
    if (snapshot == nullptr) {
        setError(errorMessage, QStringLiteral("Внутренняя ошибка снимка шаблона ПЭ"));
        return false;
    }

    QXlsx::Document xlsx(QStringLiteral("perechen.xlsx"));
    if (!xlsx.selectSheet(QStringLiteral("2"))) {
        setError(errorMessage, QStringLiteral("Не удалось открыть лист 2 шаблона ПЭ"));
        return false;
    }

    snapshot->columnWidths.clear();
    snapshot->rowHeights.clear();
    snapshot->cellsWithText.clear();
    snapshot->mergedCells.clear();
    snapshot->cellFormats.clear();

    for (int column = 1; column <= 21; ++column) {
        snapshot->columnWidths.insert(column, xlsx.columnWidth(column));
    }
    for (int row = 1; row <= 37; ++row) {
        snapshot->rowHeights.insert(row, xlsx.rowHeight(row));
    }

    QXlsx::Worksheet *worksheet = xlsx.currentWorksheet();
    if (worksheet == nullptr) {
        setError(errorMessage, QStringLiteral("Не удалось получить worksheet шаблона ПЭ"));
        return false;
    }

    snapshot->mergedCells = worksheet->mergedCells();
    const auto dimension = worksheet->dimension();
    for (int row = 1; row <= dimension.lastRow(); ++row) {
        for (int column = 1; column <= dimension.lastColumn(); ++column) {
            const QString cellName = cellNameFromPosition(row, column);
            if (auto cell = worksheet->cellAt(row, column)) {
                snapshot->cellFormats.insert(cellName, cell->format());
            }
            const QString text = xlsx.read(row, column).toString();
            if (!text.isEmpty()) {
                snapshot->cellsWithText.insert(cellName, text);
            }
        }
    }

    return true;
}

bool PeGeneratorService::populateDocument(const ProjectData &project,
                                          const QString &outputPath,
                                          QString *errorMessage) const
{
    PageContainer container;
    Page currentPage;
    const int firstPageMax = 29;
    const int otherPagesMax = 32;

    for (int i = 0; i < project.peLines.size(); ++i) {
        currentPage.pipelines.append(project.peLines.at(i));
        if ((i + 1 == firstPageMax) ||
            (i + 1 > firstPageMax && (i + 1 - firstPageMax) % otherPagesMax == 0)) {
            container.pages.append(currentPage);
            currentPage = Page();
        }
    }
    if (!currentPage.pipelines.isEmpty()) {
        container.pages.append(currentPage);
    }

    if (container.pages.isEmpty()) {
        setError(errorMessage, QStringLiteral("После разбиения не осталось страниц ПЭ"));
        return false;
    }

    const int pageCount = container.pages.size();
    for (int pageIndex = 3; pageIndex <= pageCount; ++pageIndex) {
        if (!cloneSheetTemplate(outputPath, QString::number(pageIndex), errorMessage)) {
            return false;
        }
    }

    QXlsx::Document document(outputPath);
    int currentPageIndex = 0;
    const QString designation = projectDesignation(project);
    const QString projectName = projectDisplayName(project);

    for (const Page &page : container.pages) {
        ++currentPageIndex;
        if (!document.selectSheet(QString::number(currentPageIndex))) {
            setError(errorMessage, QStringLiteral("Не удалось выбрать лист %1 файла ПЭ").arg(currentPageIndex));
            return false;
        }

        int row = 3;
        for (const peline &line : page.pipelines) {
            QXlsx::Format baseFormat;
            const QXlsx::Format::BorderStyle mediumBorder(QXlsx::Format::BorderMedium);
            const QXlsx::Format::BorderStyle thinBorder(QXlsx::Format::BorderThin);
            baseFormat.setRightBorderStyle(mediumBorder);
            baseFormat.setLeftBorderStyle(mediumBorder);
            baseFormat.setTopBorderStyle(thinBorder);
            baseFormat.setBottomBorderStyle(thinBorder);
            baseFormat.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
            baseFormat.setVerticalAlignment(QXlsx::Format::AlignVCenter);
            QFont gostFont12(QStringLiteral("GOST type B"), 12);
            baseFormat.setFont(gostFont12);

            int cols[4];
            if (currentPageIndex > 1) {
                for (int i = 0; i < 4; ++i) {
                    cols[i] = kPeColumns[1][i];
                }

                QXlsx::Format formatFilename;
                formatFilename.setRightBorderStyle(mediumBorder);
                formatFilename.setLeftBorderStyle(mediumBorder);
                formatFilename.setTopBorderStyle(thinBorder);
                formatFilename.setBottomBorderStyle(thinBorder);
                formatFilename.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
                formatFilename.setVerticalAlignment(QXlsx::Format::AlignVCenter);
                QFont gostFont20(QStringLiteral("GOST type B"), 20);
                formatFilename.setFont(gostFont20);
                document.write(36, 16, currentPageIndex);
                document.write(34, 10, designation + QStringLiteral("ПЭ3"), formatFilename);
            } else {
                for (int i = 0; i < 4; ++i) {
                    cols[i] = kPeColumns[0][i];
                }

                document.write(34, 18, pageCount + 1);
                document.write(33, 10, projectName);
                document.write(1, 3, designation);
                document.write(33, 6, project.stamps.pe.value(QStringLiteral("Разработал")));
                document.write(34, 6, project.stamps.pe.value(QStringLiteral("Проверил")));
                document.write(36, 6, project.stamps.pe.value(QStringLiteral("Нормоконтроль")));
                document.write(37, 6, project.stamps.pe.value(QStringLiteral("Утвердил")));
                document.write(35, 6, project.stamps.pe.value(QStringLiteral("Начальник отдела")));
                document.write(35, 4, departmentHeadCaption(project));
                document.write(30, 10, designation + QStringLiteral("ПЭ3"));
            }

            if (line.type == QStringLiteral("ElementGroup")) {
                QXlsx::Format groupFormat;
                groupFormat.setFontBold(false);
                groupFormat.setFontUnderline(QXlsx::Format::FontUnderlineSingle);
                groupFormat.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
                groupFormat.setVerticalAlignment(QXlsx::Format::AlignVCenter);
                groupFormat.setRightBorderStyle(mediumBorder);
                groupFormat.setLeftBorderStyle(mediumBorder);
                groupFormat.setTopBorderStyle(thinBorder);
                groupFormat.setBottomBorderStyle(thinBorder);
                QFont groupFont(QStringLiteral("GOST type B"), 14);
                groupFont.setUnderline(true);
                groupFormat.setFont(groupFont);
                document.write(row, cols[1], line.name, groupFormat);
            } else {
                document.write(row, cols[1], line.name);
            }

            document.write(row, cols[0], line.oboz, baseFormat);
            document.write(row, cols[2], line.num, baseFormat);
            document.write(row, cols[3], line.comment, baseFormat);
            ++row;
        }

        if (currentPageIndex == pageCount) {
            QXlsx::Worksheet *worksheet = document.currentWorksheet();
            if (worksheet != nullptr) {
                for (int rowIndex = 2; rowIndex <= 32; ++rowIndex) {
                    for (int col = 4; col <= 16; ++col) {
                        if (auto cell = worksheet->cellAt(rowIndex, col)) {
                            const QString cellText = document.read(rowIndex, col).toString();
                            QXlsx::Format format = cell->format();
                            format.setRightBorderStyle(QXlsx::Format::BorderMedium);
                            document.write(rowIndex, col, cellText, format);
                        }
                    }
                }
            }
        }
    }

    const int lriIndex = document.sheetNames().indexOf(QStringLiteral("ЛРИ"));
    if (lriIndex != -1) {
        document.moveSheet(QStringLiteral("ЛРИ"), document.sheetNames().size() - 1);
    }

    if (document.selectSheet(QStringLiteral("ЛРИ"))) {
        document.write(34, 12, designation + QStringLiteral("ПЭ3"));
        document.write(36, 19, pageCount + 1);

        QXlsx::Worksheet *worksheet = document.currentWorksheet();
        if (worksheet != nullptr) {
            if (auto cell = worksheet->cellAt(5, 10)) {
                const QString cellText = document.read(5, 10).toString();
                QXlsx::Format format = cell->format();
                format.setRightBorderStyle(QXlsx::Format::BorderMedium);
                document.write(6, 18, cellText, format);
                document.write(6, 19, cellText, format);
                for (int rowIndex = 5; rowIndex <= 32; ++rowIndex) {
                    for (int column = 13; column <= 17; ++column) {
                        document.write(rowIndex, column, cellText, format);
                    }
                }
            }
        }
    }

    if (!document.save()) {
        setError(errorMessage, QStringLiteral("Не удалось сохранить итоговый файл ПЭ"));
        return false;
    }

    return true;
}

QString PeGeneratorService::projectDesignation(const ProjectData &project) const
{
    if (!project.stampFields.decimalNumber.trimmed().isEmpty()) {
        return project.stampFields.decimalNumber.trimmed();
    }
    return project.projectCode.trimmed();
}

QString PeGeneratorService::projectDisplayName(const ProjectData &project) const
{
    return project.stampFields.projectName.trimmed();
}

QString PeGeneratorService::departmentHeadCaption(const ProjectData &project) const
{
    const QString altValue = project.stamps.pe.value(QStringLiteral("Начальник отдела ( альтернативное название параметра)")).trimmed();
    if (!altValue.isEmpty()) {
        return altValue;
    }
    return project.stamps.pe.value(QStringLiteral("Начальник отдела")).trimmed();
}

void PeGeneratorService::setError(QString *errorMessage, const QString &text) const
{
    if (errorMessage != nullptr) {
        *errorMessage = text;
    }
}
