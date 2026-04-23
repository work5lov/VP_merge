#include "backend/spgeneratorservice.h"

#include <QChar>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFont>

namespace {
constexpr int kSpColumns[2][6] = {
    {4, 7, 9, 14, 20, 21},
    {4, 7, 9, 14, 20, 21}
};

QString cellNameFromPosition(int row, int column)
{
    return QString("%1%2").arg(QChar('A' + column - 1)).arg(row);
}
}

bool SpGeneratorService::generate(const ProjectData &project,
                                  const QString &outputPath,
                                  QString *errorMessage) const
{
    if (project.spLines.isEmpty()) {
        setError(errorMessage, QStringLiteral("Нет данных СП для генерации"));
        return false;
    }

    if (outputPath.trimmed().isEmpty()) {
        setError(errorMessage, QStringLiteral("Не указан путь выходного файла СП"));
        return false;
    }

    if (!ensureTemplateCopied(outputPath, errorMessage)) {
        return false;
    }

    return populateDocument(project, outputPath, errorMessage);
}

bool SpGeneratorService::ensureTemplateCopied(const QString &outputPath, QString *errorMessage) const
{
    const QString sourceFilePath = QStringLiteral("specification.xlsx");
    QFile sourceFile(sourceFilePath);
    if (!sourceFile.exists()) {
        setError(errorMessage, QStringLiteral("Не найден шаблон СП: %1").arg(sourceFilePath));
        return false;
    }

    QFileInfo destinationInfo(outputPath);
    QDir destinationDir = destinationInfo.dir();
    if (!destinationDir.exists() && !destinationDir.mkpath(QStringLiteral("."))) {
        setError(errorMessage, QStringLiteral("Не удалось создать каталог для СП: %1").arg(destinationDir.absolutePath()));
        return false;
    }

    if (destinationInfo.exists() && !QFile::remove(outputPath)) {
        setError(errorMessage, QStringLiteral("Не удалось удалить существующий файл СП: %1").arg(outputPath));
        return false;
    }

    if (!sourceFile.copy(outputPath)) {
        setError(errorMessage, QStringLiteral("Не удалось скопировать шаблон СП в %1").arg(outputPath));
        return false;
    }

    return true;
}

bool SpGeneratorService::cloneSheetTemplate(const QString &filePath,
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
        setError(errorMessage, QStringLiteral("Не удалось выбрать лист %1 в файле СП").arg(sheetName));
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
        setError(errorMessage, QStringLiteral("Не удалось сохранить лист %1 в файле СП").arg(sheetName));
        return false;
    }

    return true;
}

bool SpGeneratorService::captureTemplateSnapshot(TemplateSnapshot *snapshot,
                                                 QString *errorMessage) const
{
    if (snapshot == nullptr) {
        setError(errorMessage, QStringLiteral("Внутренняя ошибка снимка шаблона СП"));
        return false;
    }

    QXlsx::Document xlsx(QStringLiteral("specification.xlsx"));
    if (!xlsx.selectSheet(QStringLiteral("2"))) {
        setError(errorMessage, QStringLiteral("Не удалось открыть лист 2 шаблона СП"));
        return false;
    }

    snapshot->columnWidths.clear();
    snapshot->rowHeights.clear();
    snapshot->cellsWithText.clear();
    snapshot->mergedCells.clear();
    snapshot->cellFormats.clear();

    for (int column = 1; column <= 20; ++column) {
        snapshot->columnWidths.insert(column, xlsx.columnWidth(column));
    }
    for (int row = 1; row <= 39; ++row) {
        snapshot->rowHeights.insert(row, xlsx.rowHeight(row));
    }

    QXlsx::Worksheet *worksheet = xlsx.currentWorksheet();
    if (worksheet == nullptr) {
        setError(errorMessage, QStringLiteral("Не удалось получить worksheet шаблона СП"));
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

bool SpGeneratorService::populateDocument(const ProjectData &project,
                                          const QString &outputPath,
                                          QString *errorMessage) const
{
    PageContainer container;
    Page currentPage;
    const int firstPageMax = 24;
    const int otherPagesMax = 29;

    for (int i = 0; i < project.spLines.size(); ++i) {
        currentPage.pispeclines.append(project.spLines.at(i));
        if ((i + 1 == firstPageMax) ||
            (i + 1 > firstPageMax && (i + 1 - firstPageMax) % otherPagesMax == 0)) {
            container.pages.append(currentPage);
            currentPage = Page();
        }
    }
    if (!currentPage.pispeclines.isEmpty()) {
        container.pages.append(currentPage);
    }

    if (container.pages.isEmpty()) {
        setError(errorMessage, QStringLiteral("После разбиения не осталось страниц СП"));
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
            setError(errorMessage, QStringLiteral("Не удалось выбрать лист %1 файла СП").arg(currentPageIndex));
            return false;
        }

        int row = 3;
        for (const specline &line : page.pispeclines) {
            int cols[6];
            if (currentPageIndex > 1) {
                for (int i = 0; i < 6; ++i) {
                    cols[i] = kSpColumns[1][i];
                }

                QXlsx::Format formatFilename;
                formatFilename.setRightBorderStyle(QXlsx::Format::BorderMedium);
                formatFilename.setLeftBorderStyle(QXlsx::Format::BorderMedium);
                formatFilename.setTopBorderStyle(QXlsx::Format::BorderMedium);
                formatFilename.setBottomBorderStyle(QXlsx::Format::BorderMedium);
                formatFilename.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
                formatFilename.setVerticalAlignment(QXlsx::Format::AlignVCenter);
                QFont gostFont(QStringLiteral("GOST type B"), 12);
                formatFilename.setFont(gostFont);
                document.write(37, 22, currentPageIndex, formatFilename);
                document.write(35, 12, designation);
                document.setColumnWidth(21, 21, 5.71094);
                document.setColumnWidth(22, 22, 5.14063);
            } else {
                for (int i = 0; i < 6; ++i) {
                    cols[i] = kSpColumns[0][i];
                }

                QXlsx::Format formatFilename;
                formatFilename.setRightBorderStyle(QXlsx::Format::BorderMedium);
                formatFilename.setLeftBorderStyle(QXlsx::Format::BorderMedium);
                formatFilename.setTopBorderStyle(QXlsx::Format::BorderMedium);
                formatFilename.setBottomBorderStyle(QXlsx::Format::BorderMedium);
                formatFilename.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
                formatFilename.setVerticalAlignment(QXlsx::Format::AlignVCenter);
                QFont gostFont(QStringLiteral("GOST type B"), 12);
                formatFilename.setFont(gostFont);
                document.write(36, 22, pageCount + 1, formatFilename);
                document.write(35, 12, projectName);
                document.write(1, 3, designation);
                document.write(35, 8, project.stamps.sp.value(QStringLiteral("Разработал")));
                document.write(36, 8, project.stamps.sp.value(QStringLiteral("Проверил")));
                document.write(38, 8, project.stamps.sp.value(QStringLiteral("Нормоконтроль")));
                document.write(39, 8, project.stamps.sp.value(QStringLiteral("Утвердил")));
                document.write(37, 8, project.stamps.sp.value(QStringLiteral("Начальник отдела")));
                document.write(37, 4, departmentHeadCaption(project));
                document.write(32, 12, designation);
            }

            if (line.underline) {
                QXlsx::Format groupFormat;
                groupFormat.setFontBold(false);
                groupFormat.setFontUnderline(QXlsx::Format::FontUnderlineSingle);
                groupFormat.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
                groupFormat.setVerticalAlignment(QXlsx::Format::AlignVCenter);
                QFont groupFont(QStringLiteral("GOST type B"), 14);
                groupFont.setUnderline(true);
                groupFormat.setFont(groupFont);
                document.write(row, cols[3], line.name, groupFormat);
            } else {
                document.write(row, cols[3], line.name);
            }

            document.write(row, cols[0], line.format);
            document.write(row, cols[1], line.pos);
            document.write(row, cols[2], line.oboz);
            document.write(row, cols[4], line.num);
            document.write(row, cols[5], line.comment);
            ++row;
        }
    }

    const int lriIndex = document.sheetNames().indexOf(QStringLiteral("ЛРИ"));
    if (lriIndex != -1) {
        document.moveSheet(QStringLiteral("ЛРИ"), document.sheetNames().size() - 1);
    }

    if (document.selectSheet(QStringLiteral("ЛРИ"))) {
        document.write(37, 19, pageCount + 1);
        document.write(35, 12, designation);
    }

    if (!document.save()) {
        setError(errorMessage, QStringLiteral("Не удалось сохранить итоговый файл СП"));
        return false;
    }

    return true;
}

QString SpGeneratorService::projectDesignation(const ProjectData &project) const
{
    if (!project.stampFields.decimalNumber.trimmed().isEmpty()) {
        return project.stampFields.decimalNumber.trimmed();
    }
    return project.projectCode.trimmed();
}

QString SpGeneratorService::projectDisplayName(const ProjectData &project) const
{
    return project.stampFields.projectName.trimmed();
}

QString SpGeneratorService::departmentHeadCaption(const ProjectData &project) const
{
    const QString altValue = project.stamps.sp.value(QStringLiteral("Начальник отдела ( альтернативное название параметра)")).trimmed();
    if (!altValue.isEmpty()) {
        return altValue;
    }
    return project.stamps.sp.value(QStringLiteral("Начальник отдела")).trimmed();
}

void SpGeneratorService::setError(QString *errorMessage, const QString &text) const
{
    if (errorMessage != nullptr) {
        *errorMessage = text;
    }
}
