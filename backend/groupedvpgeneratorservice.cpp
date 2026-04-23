#include "backend/groupedvpgeneratorservice.h"

#include <QChar>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFont>
#include <QString>

#include <algorithm>

#include "backend/xmlprojectparser.h"

namespace {
constexpr int kGroupedVpColumns[2][8] = {
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

void addVpline(QList<vpline> &lines, const QString &num, const QString &name, const QString &type)
{
    vpline line;
    line.num = num;
    line.name = name;
    line.type = type;
    lines.append(line);
}

bool isSupplementalLine(const vpline &line)
{
    return line.name.trimmed().isEmpty() && (!line.post.trimmed().isEmpty() || !line.oboz.trimmed().isEmpty());
}
}

bool GroupedVpGeneratorService::generate(const ImportSummary &importSummary,
                                         const ProjectStampSet &stamps,
                                         const ProjectStampFields &stampFields,
                                         const QString &outputPath,
                                         const XmlProjectParser &parser,
                                         QString *errorMessage) const
{
    if (importSummary.xmlFiles.isEmpty()) {
        setError(errorMessage, QStringLiteral("Нет импортированных XML для групповой ВП"));
        return false;
    }

    if (outputPath.trimmed().isEmpty()) {
        setError(errorMessage, QStringLiteral("Не указан путь выходного файла групповой ВП"));
        return false;
    }

    const QMap<QString, QList<vpline>> categories = parser.readVpCategories(importSummary.xmlFiles);
    const GroupedVpData data = buildGroupedData(categories);
    if (data.lines.isEmpty() || data.pageCount <= 0) {
        setError(errorMessage, QStringLiteral("Нет данных для формирования групповой ВП"));
        return false;
    }

    if (!ensureTemplateCopied(outputPath, errorMessage)) {
        return false;
    }

    return populateDocument(data, stamps, stampFields, outputPath, errorMessage);
}

bool GroupedVpGeneratorService::ensureTemplateCopied(const QString &outputPath, QString *errorMessage) const
{
    const QString sourceFilePath = QStringLiteral("Vedomost.xlsx");
    QFile sourceFile(sourceFilePath);
    if (!sourceFile.exists()) {
        setError(errorMessage, QStringLiteral("Не найден шаблон групповой ВП: %1").arg(sourceFilePath));
        return false;
    }

    QFileInfo destinationInfo(outputPath);
    QDir destinationDir = destinationInfo.dir();
    if (!destinationDir.exists() && !destinationDir.mkpath(QStringLiteral("."))) {
        setError(errorMessage, QStringLiteral("Не удалось создать каталог для групповой ВП: %1").arg(destinationDir.absolutePath()));
        return false;
    }

    if (destinationInfo.exists() && !QFile::remove(outputPath)) {
        setError(errorMessage, QStringLiteral("Не удалось удалить существующий файл групповой ВП: %1").arg(outputPath));
        return false;
    }

    if (!sourceFile.copy(outputPath)) {
        setError(errorMessage, QStringLiteral("Не удалось скопировать шаблон групповой ВП в %1").arg(outputPath));
        return false;
    }

    return true;
}

bool GroupedVpGeneratorService::cloneSheetTemplate(const QString &filePath,
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
        setError(errorMessage, QStringLiteral("Не удалось выбрать лист %1 в файле групповой ВП").arg(sheetName));
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
        setError(errorMessage, QStringLiteral("Не удалось сохранить лист %1 в файле групповой ВП").arg(sheetName));
        return false;
    }

    return true;
}

bool GroupedVpGeneratorService::captureTemplateSnapshot(TemplateSnapshot *snapshot,
                                                        QString *errorMessage) const
{
    if (snapshot == nullptr) {
        setError(errorMessage, QStringLiteral("Внутренняя ошибка снимка шаблона групповой ВП"));
        return false;
    }

    QXlsx::Document xlsx(QStringLiteral("Vedomost.xlsx"));
    if (!xlsx.selectSheet(QStringLiteral("2"))) {
        setError(errorMessage, QStringLiteral("Не удалось открыть лист 2 шаблона групповой ВП"));
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
        setError(errorMessage, QStringLiteral("Не удалось получить worksheet шаблона групповой ВП"));
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

GroupedVpGeneratorService::GroupedVpData GroupedVpGeneratorService::buildGroupedData(const QMap<QString, QList<vpline>> &categories) const
{
    GroupedVpData result;
    if (categories.isEmpty()) {
        return result;
    }

    const QStringList categoryOrder = categories.keys();
    QList<vpline> preparedLines;
    QMap<QString, QString> extendedObozByName;
    QMap<QString, QString> extendedPostByName;
    QList<vpline> flattened;

    for (const QString &category : categoryOrder) {
        flattened.append(categories.value(category));
    }

    for (int i = 0; i + 2 < flattened.size(); ++i) {
        const vpline &current = flattened.at(i);
        const vpline &next = flattened.at(i + 1);
        const vpline &next2 = flattened.at(i + 2);

        const bool post2 = !current.name.trimmed().isEmpty() && next.name.trimmed().isEmpty() && !next.post.trimmed().isEmpty();
        const bool post3 = post2 && next2.name.trimmed().isEmpty() && !next2.post.trimmed().isEmpty();
        const bool oboz2 = !current.name.trimmed().isEmpty() && next.name.trimmed().isEmpty() && !next.oboz.trimmed().isEmpty();
        const bool oboz3 = oboz2 && next2.name.trimmed().isEmpty() && !next2.oboz.trimmed().isEmpty();

        if (post2) {
            const QString merged = post3
                ? current.post + next.post + next2.post
                : current.post + next.post;
            if (merged.length() > extendedPostByName.value(current.name).length()) {
                extendedPostByName.insert(current.name, merged);
            }
        }

        if (oboz2) {
            const QString merged = oboz3
                ? current.oboz + QStringLiteral(" ") + next.oboz + QStringLiteral(":") + next2.oboz
                : current.oboz + QStringLiteral(" ") + next.oboz;
            if (merged.length() > extendedObozByName.value(current.name).length()) {
                extendedObozByName.insert(current.name, merged);
            }
        }
    }

    for (const QString &category : categoryOrder) {
        QList<vpline> categoryLines = categories.value(category);
        std::sort(categoryLines.begin(), categoryLines.end(), [](const vpline &a, const vpline &b) {
            if (a.name != b.name) {
                return a.name < b.name;
            }
            return a.vhodit < b.vhodit;
        });

        categoryLines.erase(std::remove_if(categoryLines.begin(), categoryLines.end(), [](const vpline &line) {
            return line.name.trimmed().isEmpty() || line.oboz.trimmed().isEmpty() || line.post.trimmed().isEmpty();
        }), categoryLines.end());

        if (categoryLines.isEmpty()) {
            continue;
        }

        addVpline(preparedLines, QString(), category, QStringLiteral("ElementGroup"));
        for (const vpline &line : categoryLines) {
            vpline updated = line;
            const QString mergedOboz = extendedObozByName.value(line.name);
            const QString mergedPost = extendedPostByName.value(line.name);
            if (!mergedOboz.isEmpty()) {
                updated.oboz = mergedOboz;
            }
            if (!mergedPost.isEmpty()) {
                updated.post = mergedPost;
            }
            preparedLines.append(updated);
        }
        addVpline(preparedLines, QString(), QString(), QString());
    }

    if (preparedLines.isEmpty()) {
        return result;
    }

    QList<vpline> aggregated;
    aggregated.append(preparedLines.first());
    int sum = 0;
    for (int i = 1; i < preparedLines.size() - 1; ++i) {
        const vpline &current = preparedLines.at(i);
        const vpline &previous = preparedLines.at(i - 1);
        const vpline &next = preparedLines.at(i + 1);

        if (current.name == next.name) {
            aggregated.append(current);
            sum += current.num.toInt();
        }
        if ((current.name != next.name) && (current.name == previous.name)) {
            aggregated.append(current);
            sum += current.num.toInt();
            addVpline(aggregated, QString::number(sum), QString(), QString());
            sum = 0;
        }
        if (current.name.trimmed().isEmpty() || current.type == QStringLiteral("ElementGroup")) {
            aggregated.append(current);
        }
    }
    aggregated.append(preparedLines.last());

    QList<vpline> expanded;
    for (const vpline &line : aggregated) {
        if (line.post.length() > 25) {
            const QStringList postLines = line.post.split(QLatin1Char(':'), Qt::KeepEmptyParts);
            for (int i = 0; i < postLines.size(); ++i) {
                vpline extra = line;
                if (i > 0) {
                    extra.num.clear();
                    extra.oboz.clear();
                    extra.name.clear();
                    extra.comment.clear();
                    extra.underline = false;
                    extra.type.clear();
                    extra.merge = false;
                    extra.vhodit.clear();
                }
                extra.post = postLines.at(i);
                expanded.append(extra);
            }
        } else {
            expanded.append(line);
        }
    }

    QMap<int, QList<vpline>> components;
    int componentCounter = 0;
    QList<vpline> currentComponent;
    bool groupNameWritten = false;

    for (const vpline &line : expanded) {
        if (line.type == QStringLiteral("ElementGroup")) {
            currentComponent.append(line);
            groupNameWritten = false;
            continue;
        }

        if (!line.name.trimmed().isEmpty() && !line.post.trimmed().isEmpty()) {
            if (groupNameWritten) {
                components.insert(componentCounter++, currentComponent);
                currentComponent.clear();
                currentComponent.append(line);
            } else {
                currentComponent.append(line);
                groupNameWritten = true;
            }
            continue;
        }

        currentComponent.append(line);
        if (line.name.trimmed().isEmpty() && line.post.trimmed().isEmpty() && line.num.trimmed().isEmpty()) {
            components.insert(componentCounter++, currentComponent);
            currentComponent.clear();
        }
    }
    if (!currentComponent.isEmpty()) {
        components.insert(componentCounter, currentComponent);
    }

    const int firstPageMax = 22;
    const int otherPagesMax = 28;

    int currentLineCount = 0;
    int currentPage = 1;
    QList<vpline> bufferedComponent;
    for (auto it = components.cbegin(); it != components.cend(); ++it) {
        if (!bufferedComponent.isEmpty()) {
            result.lines.append(bufferedComponent);
            currentLineCount += bufferedComponent.size();
            bufferedComponent.clear();
        }

        const QList<vpline> componentLines = it.value();
        const int pageCapacity = currentPage == 1 ? firstPageMax : otherPagesMax;
        if (currentLineCount + componentLines.size() > pageCapacity) {
            if (!result.lines.isEmpty() && !result.lines.last().name.trimmed().isEmpty()) {
                addVpline(result.lines, QString(), QString(), QString());
            }
            ++result.pageCount;
            currentPage = result.pageCount + 1;
            currentLineCount = 0;
            bufferedComponent = componentLines;
        } else {
            result.lines.append(componentLines);
            currentLineCount += componentLines.size();
        }
    }

    if (!result.lines.isEmpty()) {
        result.pageCount = 1;
        int rowCount = 0;
        for (const vpline &line : result.lines) {
            const int limit = result.pageCount == 1 ? firstPageMax : otherPagesMax;
            if (rowCount >= limit) {
                ++result.pageCount;
                rowCount = 0;
            }
            ++rowCount;
        }
        if (result.pageCount == 0) {
            result.pageCount = 1;
        }
    }

    if (result.pageCount == 0 && !result.lines.isEmpty()) {
        result.pageCount = 1;
    }

    return result;
}

bool GroupedVpGeneratorService::populateDocument(const GroupedVpData &data,
                                                 const ProjectStampSet &stamps,
                                                 const ProjectStampFields &stampFields,
                                                 const QString &outputPath,
                                                 QString *errorMessage) const
{
    PageContainer container;
    Page currentPage;
    int currentPageNumber = 1;
    int currentPageCount = 0;
    const int firstPageMax = 22;
    const int otherPagesMax = 28;

    for (const vpline &line : data.lines) {
        const int pageCapacity = currentPageNumber == 1 ? firstPageMax : otherPagesMax;
        if (currentPageCount >= pageCapacity) {
            container.pages.append(currentPage);
            currentPage = Page();
            currentPageCount = 0;
            ++currentPageNumber;
        }
        currentPage.pivplines.append(line);
        ++currentPageCount;
    }
    if (!currentPage.pivplines.isEmpty()) {
        container.pages.append(currentPage);
    }

    if (container.pages.isEmpty()) {
        setError(errorMessage, QStringLiteral("После разбиения не осталось страниц групповой ВП"));
        return false;
    }

    const int pageCount = container.pages.size();
    for (int pageIndex = 3; pageIndex <= pageCount; ++pageIndex) {
        if (!cloneSheetTemplate(outputPath, QString::number(pageIndex), errorMessage)) {
            return false;
        }
    }

    QXlsx::Document document(outputPath);
    int pageIndex = 0;
    const QString designation = projectDesignation(stampFields, ImportSummary());
    const QString projectName = projectDisplayName(stampFields);

    for (const Page &page : container.pages) {
        ++pageIndex;
        if (!document.selectSheet(QString::number(pageIndex))) {
            setError(errorMessage, QStringLiteral("Не удалось выбрать лист %1 файла групповой ВП").arg(pageIndex));
            return false;
        }

        int row = 4;
        const int *cols = pageIndex > 1 ? kGroupedVpColumns[1] : kGroupedVpColumns[0];
        if (pageIndex > 1) {
            document.write(36, 21, pageIndex);
            document.write(34, 15, designation + QStringLiteral("ВП"));
        } else {
            document.write(36, 29, pageCount + 1);
            document.write(35, 17, projectName);
            document.write(1, 3, designation);
            document.write(35, 13, stamps.vp.value(QStringLiteral("Разработал")));
            document.write(36, 13, stamps.vp.value(QStringLiteral("Проверил")));
            document.write(38, 13, stamps.vp.value(QStringLiteral("Нормоконтроль")));
            document.write(39, 13, stamps.vp.value(QStringLiteral("Утвердил")));
            document.write(37, 13, stamps.vp.value(QStringLiteral("Начальник отдела")));
            document.write(37, 9, departmentHeadCaption(stamps, stampFields));
            document.write(32, 17, designation + QStringLiteral("ВП"));
        }

        const int rowCountBase = pageIndex < 2 ? 25 : 31;
        const int lastRow = pageIndex < 2 ? 24 : 30;
        document.write(3, 4, QString::number((pageIndex - 1) * rowCountBase + 1));
        document.write(lastRow + 2, 4, QString::number((pageIndex - 1) * rowCountBase + lastRow));
        document.write(lastRow + 3, 4, QString::number((pageIndex - 1) * rowCountBase + lastRow + 1));

        for (const vpline &line : page.pivplines) {
            document.write(row, 4, QString::number((pageIndex - 1) * rowCountBase + row - 2));

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
                    document.write(row, cols[4], line.vhodit);
                }
            }

            document.write(row, cols[1], line.kod);

            if (line.oboz.length() > 30) {
                QXlsx::Format wrappedFormat;
                wrappedFormat.setTextWrap(true);
                wrappedFormat.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
                wrappedFormat.setVerticalAlignment(QXlsx::Format::AlignVCenter);
                QFont gostFont(QStringLiteral("GOST type B"), 10);
                wrappedFormat.setFont(gostFont);
                document.write(row, cols[2], line.oboz, wrappedFormat);
            } else {
                document.write(row, cols[2], line.oboz);
            }

            if (line.post.length() > 25) {
                QXlsx::Format wrappedFormat;
                wrappedFormat.setTextWrap(true);
                wrappedFormat.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
                wrappedFormat.setVerticalAlignment(QXlsx::Format::AlignVCenter);
                QFont gostFont(QStringLiteral("GOST type B"), 10);
                wrappedFormat.setFont(gostFont);
                document.write(row, cols[3], line.post, wrappedFormat);
            } else {
                document.write(row, cols[3], line.post);
            }

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
        setError(errorMessage, QStringLiteral("Не удалось сохранить итоговый файл групповой ВП"));
        return false;
    }

    return true;
}

QString GroupedVpGeneratorService::projectDesignation(const ProjectStampFields &stampFields, const ImportSummary &importSummary) const
{
    if (!stampFields.decimalNumber.trimmed().isEmpty()) {
        return stampFields.decimalNumber.trimmed();
    }
    for (const QString &code : importSummary.projectCodes) {
        if (!code.trimmed().isEmpty()) {
            return code.trimmed();
        }
    }
    return QStringLiteral("project");
}

QString GroupedVpGeneratorService::projectDisplayName(const ProjectStampFields &stampFields) const
{
    return stampFields.projectName.trimmed();
}

QString GroupedVpGeneratorService::departmentHeadCaption(const ProjectStampSet &stamps, const ProjectStampFields &stampFields) const
{
    const QString altValue = stamps.vp.value(QStringLiteral("Начальник отдела ( альтернативное название параметра)")).trimmed();
    if (!altValue.isEmpty()) {
        return altValue;
    }
    if (!stampFields.departmentHeadAlt.trimmed().isEmpty()) {
        return stampFields.departmentHeadAlt.trimmed();
    }
    const QString primary = stamps.vp.value(QStringLiteral("Начальник отдела")).trimmed();
    if (!primary.isEmpty()) {
        return primary;
    }
    return QStringLiteral("Начальник отдела");
}

void GroupedVpGeneratorService::setError(QString *errorMessage, const QString &text) const
{
    if (errorMessage != nullptr) {
        *errorMessage = text;
    }
}
