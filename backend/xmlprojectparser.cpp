#include "backend/xmlprojectparser.h"

#include <QDebug>
#include <QFile>
#include <QXmlStreamReader>

namespace {
QString readBoolString(const QString &value)
{
    return value.trimmed();
}

bool isTrueValue(const QString &value)
{
    return readBoolString(value).compare("True", Qt::CaseInsensitive) == 0;
}
}

QString XmlProjectParser::extractProjectCode(const QString &filePath) const
{
    QString projectCode;
    parseSpLines(filePath, &projectCode);
    return projectCode;
}

ProjectData XmlProjectParser::parseProject(const QString &filePath) const
{
    ProjectData result;
    result.sourceXmlPath = filePath;
    result.stamps = parseStamps(filePath);
    result.spLines = parseSpLines(filePath, &result.projectCode);
    result.peLines = parsePeLines(filePath);
    result.vpLines = parseVpLines(filePath, result.projectCode);
    return result;
}

QMap<QString, QList<vpline>> XmlProjectParser::readVpCategories(const QStringList &xmlFiles) const
{
    QMap<QString, QList<vpline>> categoriesMap;

    for (const QString &fileName : xmlFiles) {
        const QString projectCode = extractProjectCode(fileName);
        QFile file(fileName);
        if (!file.open(QFile::ReadOnly | QFile::Text)) {
            qWarning() << "Не удалось открыть файл" << fileName;
            continue;
        }

        QXmlStreamReader reader(&file);
        QString currentGroup;

        while (!reader.atEnd() && !reader.hasError()) {
            const auto token = reader.readNext();
            if (token != QXmlStreamReader::StartElement) {
                continue;
            }

            if (reader.name() != QLatin1String("vpline")) {
                continue;
            }

            const QString elementType = reader.attributes().value("type").toString();
            if (elementType == QLatin1String("ElementGroup")) {
                currentGroup = reader.attributes().value("name").toString();
                continue;
            }

            vpline element;
            element.num = reader.attributes().value("num").toString();
            element.oboz = reader.attributes().value("oboz").toString();
            element.kod = reader.attributes().value("kod").toString();
            element.name = reader.attributes().value("name").toString();
            element.comment = reader.attributes().value("comment").toString();
            element.underline = isTrueValue(reader.attributes().value("underline").toString());
            element.type = elementType;
            element.merge = isTrueValue(reader.attributes().value("merge").toString());
            element.vhodit = reader.attributes().value("vhodit").toString();
            element.post = reader.attributes().value("post").toString();
            if (element.vhodit.isEmpty()) {
                element.vhodit = projectCode;
            }

            categoriesMap[currentGroup].append(element);
        }

        if (reader.hasError()) {
            qWarning() << "Ошибка разбора XML:" << reader.errorString() << fileName;
        }
    }

    return categoriesMap;
}

ProjectStampSet XmlProjectParser::parseStamps(const QString &filePath) const
{
    ProjectStampSet result;

    QFile file(filePath);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        qWarning() << "Ошибка открытия файла" << filePath;
        return result;
    }

    QXmlStreamReader xmlReader(&file);
    QString currentStampType;
    while (!xmlReader.atEnd() && !xmlReader.hasError()) {
        const auto token = xmlReader.readNext();
        if (token != QXmlStreamReader::StartElement) {
            continue;
        }

        if (xmlReader.name() == QLatin1String("Stamp")) {
            currentStampType = xmlReader.attributes().value("document").toString();
            continue;
        }

        if (xmlReader.name() != QLatin1String("param")) {
            continue;
        }

        const QString paramName = xmlReader.attributes().value("name").toString();
        const QString paramValue = xmlReader.attributes().value("value").toString();
        if (currentStampType == QLatin1String("VP")) {
            result.vp[paramName] = paramValue;
        } else if (currentStampType == QLatin1String("PE")) {
            result.pe[paramName] = paramValue;
        } else if (currentStampType == QLatin1String("SP")) {
            result.sp[paramName] = paramValue;
        }
    }

    if (xmlReader.hasError()) {
        qWarning() << "Ошибка парсинга XML:" << xmlReader.errorString() << filePath;
    }

    return result;
}

QList<peline> XmlProjectParser::parsePeLines(const QString &filePath) const
{
    QList<peline> result;

    QFile file(filePath);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        qWarning() << "Ошибка открытия файла" << filePath;
        return result;
    }

    QXmlStreamReader reader(&file);
    while (!reader.atEnd() && !reader.hasError()) {
        const auto token = reader.readNext();
        if (token != QXmlStreamReader::StartElement || reader.name() != QLatin1String("peline")) {
            continue;
        }

        peline line;
        line.num = reader.attributes().value("num").toString();
        line.oboz = reader.attributes().value("oboz").toString();
        line.name = reader.attributes().value("name").toString();
        line.comment = reader.attributes().value("comment").toString();
        line.underline = isTrueValue(reader.attributes().value("underline").toString());
        line.type = reader.attributes().value("type").toString();
        result.append(line);
    }

    if (reader.hasError()) {
        qWarning() << "Ошибка парсинга peline:" << reader.errorString() << filePath;
    }

    return result;
}

QList<specline> XmlProjectParser::parseSpLines(const QString &filePath, QString *projectCode) const
{
    QList<specline> result;

    QFile file(filePath);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        qWarning() << "Ошибка открытия файла" << filePath;
        return result;
    }

    QXmlStreamReader reader(&file);
    while (!reader.atEnd() && !reader.hasError()) {
        const auto token = reader.readNext();
        if (token != QXmlStreamReader::StartElement || reader.name() != QLatin1String("specline")) {
            continue;
        }

        specline line;
        line.num = reader.attributes().value("num").toString();
        line.oboz = reader.attributes().value("oboz").toString();
        line.name = reader.attributes().value("name").toString();
        line.comment = reader.attributes().value("comment").toString();
        line.underline = isTrueValue(reader.attributes().value("underline").toString());
        line.type = reader.attributes().value("type").toString();
        line.merge = isTrueValue(reader.attributes().value("merge").toString());
        line.needtu = isTrueValue(reader.attributes().value("needtu").toString());
        line.GroupLine1 = reader.attributes().value("GroupLine1").toString();
        line.GroupLine2 = reader.attributes().value("GroupLine2").toString();
        line.format = reader.attributes().value("format").toString();
        line.pos = reader.attributes().value("pos").toString();
        line.posinelement = reader.attributes().value("posinelement").toString();
        result.append(line);

        if (projectCode && line.name == QLatin1String("Перечень элементов")) {
            *projectCode = line.oboz.left(qMax(0, line.oboz.length() - 3));
        }
    }

    if (reader.hasError()) {
        qWarning() << "Ошибка парсинга specline:" << reader.errorString() << filePath;
    }

    return result;
}

QList<vpline> XmlProjectParser::parseVpLines(const QString &filePath, const QString &fallbackProjectCode) const
{
    QList<vpline> result;

    QFile file(filePath);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        qWarning() << "Ошибка открытия файла" << filePath;
        return result;
    }

    QXmlStreamReader reader(&file);
    while (!reader.atEnd() && !reader.hasError()) {
        const auto token = reader.readNext();
        if (token != QXmlStreamReader::StartElement || reader.name() != QLatin1String("vpline")) {
            continue;
        }

        vpline line;
        line.num = reader.attributes().value("num").toString();
        line.oboz = reader.attributes().value("oboz").toString();
        line.kod = reader.attributes().value("kod").toString();
        line.name = reader.attributes().value("name").toString();
        line.comment = reader.attributes().value("comment").toString();
        line.underline = isTrueValue(reader.attributes().value("underline").toString());
        line.type = reader.attributes().value("type").toString();
        line.merge = isTrueValue(reader.attributes().value("merge").toString());
        line.vhodit = reader.attributes().value("vhodit").toString();
        line.post = reader.attributes().value("post").toString();
        if (line.vhodit.isEmpty()) {
            line.vhodit = fallbackProjectCode;
        }
        result.append(line);
    }

    if (reader.hasError()) {
        qWarning() << "Ошибка парсинга vpline:" << reader.errorString() << filePath;
    }

    return result;
}

PageContainer XmlProjectParser::paginatePeLines(const QList<peline> &lines) const
{
    PageContainer container;
    const int firstPageMax = 23;
    const int otherPagesMax = 29;

    Page currentPage;
    for (int i = 0; i < lines.size(); ++i) {
        if ((i == firstPageMax && currentPage.pipelines.size() == firstPageMax)
            || (i > firstPageMax && (i - firstPageMax) % otherPagesMax == 0)) {
            container.pages.append(currentPage);
            currentPage = Page();
        }
        currentPage.pipelines.append(lines.at(i));
    }

    if (!currentPage.pipelines.isEmpty()) {
        container.pages.append(currentPage);
    }

    return container;
}

PageContainer XmlProjectParser::paginateSpLines(const QList<specline> &lines) const
{
    PageContainer container;
    const int firstPageMax = 23;
    const int otherPagesMax = 29;

    Page currentPage;
    for (int i = 0; i < lines.size(); ++i) {
        if ((i == firstPageMax && currentPage.pispeclines.size() == firstPageMax)
            || (i > firstPageMax && (i - firstPageMax) % otherPagesMax == 0)) {
            container.pages.append(currentPage);
            currentPage = Page();
        }
        currentPage.pispeclines.append(lines.at(i));
    }

    if (!currentPage.pispeclines.isEmpty()) {
        container.pages.append(currentPage);
    }

    if (!container.pages.isEmpty()) {
        Page &lastPage = container.pages.last();
        if (lastPage.pispeclines.size() >= 6) {
            const int startIndex = lastPage.pispeclines.size() - 6;
            auto iter = lastPage.pispeclines.begin() + startIndex;
            lastPage.pispeclines.erase(iter, iter + 6);
        } else {
            lastPage.pispeclines.clear();
        }
    }

    return container;
}

PageContainer XmlProjectParser::paginateVpLines(const QList<vpline> &lines) const
{
    PageContainer container;
    const int firstPageMax = 23;
    const int otherPagesMax = 29;

    Page currentPage;
    for (int i = 0; i < lines.size(); ++i) {
        if ((i == firstPageMax - 1 && currentPage.pivplines.size() == firstPageMax - 1)
            || (i > firstPageMax && (i - firstPageMax) % otherPagesMax == 0)) {
            container.pages.append(currentPage);
            currentPage = Page();
        }
        currentPage.pivplines.append(lines.at(i));
    }

    if (!currentPage.pivplines.isEmpty()) {
        container.pages.append(currentPage);
    }

    return container;
}
