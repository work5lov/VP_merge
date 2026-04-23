#ifndef HEADLESSDOCUMENTEXPORTSERVICE_H
#define HEADLESSDOCUMENTEXPORTSERVICE_H

#include "backend/documentexportresult.h"
#include "backend/documentrenderdata.h"

class HeadlessDocumentExportService
{
public:
    DocumentExportResult exportPdf(const DocumentRenderSnapshot &snapshot,
                                   const DocumentRenderData &data) const;
};

#endif // HEADLESSDOCUMENTEXPORTSERVICE_H
