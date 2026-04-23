#ifndef DOCUMENTEXPORTRESULT_H
#define DOCUMENTEXPORTRESULT_H

#include <QString>

struct DocumentExportResult
{
    bool success = false;
    QString documentType;
    QString outputPath;
    QString errorMessage;
    int exportedPageCount = 0;
};

#endif // DOCUMENTEXPORTRESULT_H
