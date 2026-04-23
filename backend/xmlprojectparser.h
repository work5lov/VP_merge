#ifndef XMLPROJECTPARSER_H
#define XMLPROJECTPARSER_H

#include <QString>
#include <QStringList>
#include <QMap>
#include <QList>

#include "backend/projecttypes.h"

class XmlProjectParser
{
public:
    QString extractProjectCode(const QString &filePath) const;
    ProjectData parseProject(const QString &filePath) const;
    QMap<QString, QList<vpline>> readVpCategories(const QStringList &xmlFiles) const;
    PageContainer paginatePeLines(const QList<peline> &lines) const;
    PageContainer paginateSpLines(const QList<specline> &lines) const;
    PageContainer paginateVpLines(const QList<vpline> &lines) const;

private:
    ProjectStampSet parseStamps(const QString &filePath) const;
    QList<peline> parsePeLines(const QString &filePath) const;
    QList<specline> parseSpLines(const QString &filePath, QString *projectCode) const;
    QList<vpline> parseVpLines(const QString &filePath, const QString &fallbackProjectCode) const;
};

#endif // XMLPROJECTPARSER_H
