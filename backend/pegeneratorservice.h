#ifndef PEGENERATORSERVICE_H
#define PEGENERATORSERVICE_H

#include <QList>
#include <QMap>
#include <QString>

#include "backend/projecttypes.h"
#include "xlsxcellrange.h"
#include "xlsxdocument.h"
#include "xlsxformat.h"

class PeGeneratorService
{
public:
    bool generate(const ProjectData &project,
                  const QString &outputPath,
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

    bool ensureTemplateCopied(const QString &outputPath, QString *errorMessage) const;
    bool cloneSheetTemplate(const QString &filePath,
                            const QString &sheetName,
                            QString *errorMessage) const;
    bool captureTemplateSnapshot(TemplateSnapshot *snapshot,
                                 QString *errorMessage) const;
    bool populateDocument(const ProjectData &project,
                          const QString &outputPath,
                          QString *errorMessage) const;
    QString projectDesignation(const ProjectData &project) const;
    QString projectDisplayName(const ProjectData &project) const;
    QString departmentHeadCaption(const ProjectData &project) const;
    void setError(QString *errorMessage, const QString &text) const;
};

#endif // PEGENERATORSERVICE_H
