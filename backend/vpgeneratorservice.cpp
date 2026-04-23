#include "backend/vpgeneratorservice.h"

#include <QChar>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFont>

namespace {
constexpr int kVpColumns[2][8] = {
    {5, 6, 7, 8, 14, 19, 25, 28},
    {5, 6, 7, 8, 12, 16, 19, 20}
};

QString cellNameFromPosition(int row, int column)
{
    QString columnName;
    int currentColumn = column;
    while (currentColumn > 0) {
        const int remainder = (currentColumn - 1) % 26;
        columnName.prepend(QChar('A' + remainder));
        currentColumn = (currentColumn - 1) / 26;
    }
    return QString("%1%2").arg(columnName).arg(row);
}
}

bool VpGeneratorService::generate(const ProjectData &project,
                                  const QString &outputPath,
                                  QString *errorMessage) const
{
    if (project.vpLines.isEmpty()) {
        setError(errorMessage, QStringLiteral("Нет данных ВП для генерации"));
        return false;
    }

    if (outputPath.trimmed().isEmpty()) {
        setError(errorMessage, QStringLiteral("Не указан путь выходного файла ВП"));
        return false;
    }

    if (!ensureTemplateCopied(outputPath, errorMessage)) {
        return false;
    }

    return populateDocument(project, outputPath, errorMessage);
}

bool VpGeneratorService::ensureTemplateCopied(const QString &outputPath, QString *errorMessage) const
{
    const QString sourceFilePath = QStringLiteral("Vedomost.xlsx");
    QFile sourceFile(sourceFilePath);
    if (!sourceFile.exists()) {
        setError(errorMessage, QStringLiteral("Не найден шаблон ВП: %1").arg(sourceFilePath));
        return false;
    }

    QFileInfo destinationInfo(outputPath);
    QDir destinationDir = destinationInfo.dir();
    if (!destinationDir.exists() && !destinationDir.mkpath(QStringLiteral("."))) {
        setError(errorMessage, QStringLiteral("Не удалось создать каталог для ВП: %1").arg(destinationDir.absolutePath()));
        return false;
    }

    if (destinationInfo.exists() && !QFile::remove(outputPath)) {
        setError(errorMessage, QStringLiteral("Не удалось удалить существующий файл ВП: %1").arg(outputPath));
        return false;
    }

    if (!sourceFile.copy(outputPath)) {
        setError(errorMessage, QStringLiteral("Не удалось скопировать шаблон ВП в %1").arg(outputPath));
        return false;
    }

    return true;
}

bool VpGeneratorService::cloneSheetTemplate(const QString &filePath,
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
        setError(errorMessage, QStringLiteral("Не удалось выбрать лист %1 в файле ВП").arg(sheetName));
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
        setError(errorMessage, QStringLiteral("Не удалось сохранить лист %1 в файле ВП").arg(sheetName));
        return false;
    }

    return true;
}

bool VpGeneratorService::captureTemplateSnapshot(TemplateSnapshot *snapshot,
                                                 QString *errorMessage) const
{
    if (snapshot == nullptr) {
        setError(errorMessage, QStringLiteral("Внутренняя ошибка снимка шаблона ВП"));
        return false;
    }

    QXlsx::Document xlsx(QStringLiteral("Vedomost.xlsx"));
    if (!xlsx.selectSheet(QStringLiteral("2"))) {
        setError(errorMessage, QStringLiteral("Не удалось открыть лист 2 шаблона ВП"));
        return false;
    }

    snapshot->columnWidths.clear();
    snapshot->rowHeights.clear();
    snapshot->cellsWithText.clear();
    snapshot->mergedCells.clear();
    snapshot->cellFormats.clear();

    for (int column = 1; column <= 29; ++column) {
        snapshot->columnWidths.insert(column, xlsx.columnWidth(column));
    }
    for (int row = 1; row <= 39; ++row) {
        snapshot->rowHeights.insert(row, xlsx.rowHeight(row));
    }

    QXlsx::Worksheet *worksheet = xlsx.currentWorksheet();
    if (worksheet == nullptr) {
        setError(errorMessage, QStringLiteral("Не удалось получить worksheet шаблона ВП"));
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

bool VpGeneratorService::populateDocument(const ProjectData &project,
                                          const QString &outputPath,
                                          QString *errorMessage) const
{
    PageContainer container;
    Page currentPage;
    const int firstPageMax = 23;
    const int otherPagesMax = 29;

    for (int i = 0; i < project.vpLines.size(); ++i) {
        currentPage.pivplines.append(project.vpLines.at(i));
        if ((i + 1 == firstPageMax) ||
            (i + 1 > firstPageMax && (i + 1 - firstPageMax) % otherPagesMax == 0)) {
            container.pages.append(currentPage);
            currentPage = Page();
        }
    }
    if (!currentPage.pivplines.isEmpty()) {
        container.pages.append(currentPage);
    }

    if (container.pages.isEmpty()) {
        setError(errorMessage, QStringLiteral("После разбиения не осталось страниц ВП"));
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
            setError(errorMessage, QStringLiteral("Не удалось выбрать лист %1 файла ВП").arg(currentPageIndex));
            return false;
        }

        int row = 4;
        for (const vpline &line : page.pivplines) {
            int cols[8];
            if (currentPageIndex > 1) {
                for (int i = 0; i < 8; ++i) {
                    cols[i] = kVpColumns[1][i];
                }
                document.write(36, 21, currentPageIndex);
                document.write(34, 15, designation + QStringLiteral("ВП"));
            } else {
                for (int i = 0; i < 8; ++i) {
                    cols[i] = kVpColumns[0][i];
                }
                document.write(36, 29, pageCount + 1);
                document.write(35, 17, projectName);
                document.write(1, 3, designation);
                document.write(35, 13, project.stamps.vp.value(QStringLiteral("Разработал")));
                document.write(36, 13, project.stamps.vp.value(QStringLiteral("Проверил")));
                document.write(38, 13, project.stamps.vp.value(QStringLiteral("Нормоконтроль")));
                document.write(39, 13, project.stamps.vp.value(QStringLiteral("Утвердил")));
                document.write(37, 13, project.stamps.vp.value(QStringLiteral("Начальник отдела")));
                document.write(37, 9, departmentHeadCaption(project));
                document.write(32, 17, designation + QStringLiteral("ВП"));
            }

            if (line.type == QStringLiteral("ElementGroup")) {
                QXlsx::Format groupFormat;
                groupFormat.setFontBold(false);
                groupFormat.setFontUnderline(QXlsx::Format::FontUnderlineSingle);
                groupFormat.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
                groupFormat.setVerticalAlignment(QXlsx::Format::AlignVCenter);
                QFont groupFont(QStringLiteral("GOST type B"), 14);
                groupFont.setUnderline(true);
                groupFormat.setFont(groupFont);
                document.write(row, cols[0], line.name, groupFormat);
            } else {
                document.write(row, cols[0], line.name);
                if (!line.name.trimmed().isEmpty()) {
                    document.write(row, cols[4], line.vhodit.trimmed().isEmpty() ? designation : line.vhodit);
                }
            }

            document.write(row, cols[1], line.kod);
            document.write(row, cols[2], line.oboz);
            document.write(row, cols[3], line.post);
            document.write(row, cols[5], line.num);
            document.write(row, cols[6], line.num);
            document.write(row, cols[7], line.comment);
            ++row;
        }
    }

    const QString changeSheetName = QStringLiteral("Лист регистрации изменений");
    const int changeSheetIndex = document.sheetNames().indexOf(changeSheetName);
    if (document.selectSheet(changeSheetName)) {
        document.write(37, 19, pageCount + 1);
        document.write(35, 12, designation + QStringLiteral("ВП"));
    }
    if (changeSheetIndex != -1) {
        document.moveSheet(changeSheetName, document.sheetNames().size() - 1);
    }

    if (!document.save()) {
        setError(errorMessage, QStringLiteral("Не удалось сохранить итоговый файл ВП"));
        return false;
    }

    return true;
}

QString VpGeneratorService::projectDesignation(const ProjectData &project) const
{
    if (!project.stampFields.decimalNumber.trimmed().isEmpty()) {
        return project.stampFields.decimalNumber.trimmed();
    }
    return project.projectCode.trimmed();
}

QString VpGeneratorService::projectDisplayName(const ProjectData &project) const
{
    return project.stampFields.projectName.trimmed();
}

QString VpGeneratorService::departmentHeadCaption(const ProjectData &project) const
{
    const QString altValue = project.stamps.vp.value(QStringLiteral("Начальник отдела ( альтернативное название параметра)")).trimmed();
    if (!altValue.isEmpty()) {
        return altValue;
    }
    return QStringLiteral("Начальник отдела");
}

void VpGeneratorService::setError(QString *errorMessage, const QString &text) const
{
    if (errorMessage != nullptr) {
        *errorMessage = text;
    }
}
