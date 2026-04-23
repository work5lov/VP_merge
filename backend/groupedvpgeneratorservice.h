#ifndef GROUPEDVPGENERATORSERVICE_H
#define GROUPEDVPGENERATORSERVICE_H

#include <QList>
#include <QMap>
#include <QString>
#include <QStringList>

#include "backend/projecttypes.h"
#include "xlsxcellrange.h"
#include "xlsxdocument.h"
#include "xlsxformat.h"

class XmlProjectParser;

class GroupedVpGeneratorService
{
public:
    bool generate(const ImportSummary &importSummary,
                  const ProjectStampSet &stamps,
                  const ProjectStampFields &stampFields,
                  const QString &outputPath,
                  const XmlProjectParser &parser,
                  QString *errorMessage = nullptr) const;

private:
    struct TemplateSnapshot
    {
        QMap<int, double> columnWidths;
        QMap<int, double> rowHeights;
        QMap<QString, QString> cellsWithText;
        QList<QXlsx::CellRange> mergedCells;
        QMap<QString, QXlsx::Format> cellFormats;
    };

    struct GroupedVpData
    {
        QList<vpline> lines;
        int pageCount = 0;
    };

    bool ensureTemplateCopied(const QString &outputPath, QString *errorMessage) const;
    bool cloneSheetTemplate(const QString &filePath,
                            const QString &sheetName,
                            QString *errorMessage) const;
    bool captureTemplateSnapshot(TemplateSnapshot *snapshot,
                                 QString *errorMessage) const;
    bool populateDocument(const GroupedVpData &data,
                          const ProjectStampSet &stamps,
                          const ProjectStampFields &stampFields,
                          const QString &outputPath,
                          QString *errorMessage) const;
    GroupedVpData buildGroupedData(const QMap<QString, QList<vpline>> &categories) const;
    QString projectDesignation(const ProjectStampFields &stampFields, const ImportSummary &importSummary) const;
    QString projectDisplayName(const ProjectStampFields &stampFields) const;
    QString departmentHeadCaption(const ProjectStampSet &stamps, const ProjectStampFields &stampFields) const;
    void setError(QString *errorMessage, const QString &text) const;
};

#endif // GROUPEDVPGENERATORSERVICE_H
