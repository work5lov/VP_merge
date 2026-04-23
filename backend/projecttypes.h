#ifndef PROJECTTYPES_H
#define PROJECTTYPES_H

#include <QList>
#include <QMap>
#include <QString>
#include <QStringList>

#include "Structures.h"

struct ProjectStampSet
{
    QMap<QString, QString> pe;
    QMap<QString, QString> sp;
    QMap<QString, QString> vp;
};

struct ProjectStampFields
{
    QString decimalNumber;
    QString projectName;
    QString developer;
    QString checker;
    QString approver;
    QString normControl;
    QString departmentHead;
    QString departmentHeadAlt;
};

struct ProjectData
{
    QString sourceXmlPath;
    QString projectCode;
    ProjectStampSet stamps;
    ProjectStampFields stampFields;
    QList<peline> peLines;
    QList<specline> spLines;
    QList<vpline> vpLines;
};

struct ImportSummary
{
    QStringList xmlFiles;
    QStringList projectCodes;
};

struct GenerationArtifacts
{
    QString outputDirectory;
    QString documentationDirectory;
    QString pePath;
    QString spPath;
    QString vpPath;
    QString groupedVpPath;
    QString pePdfPath;
    QString spPdfPath;
    QString vpPdfPath;
    QString groupedVpPdfPath;
    bool peGenerated = false;
    bool spGenerated = false;
    bool vpGenerated = false;
    bool groupedVpGenerated = false;
};

#endif // PROJECTTYPES_H
