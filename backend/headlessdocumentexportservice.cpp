#include "backend/headlessdocumentexportservice.h"

#include <QString>

DocumentExportResult HeadlessDocumentExportService::exportPdf(const DocumentRenderSnapshot &snapshot,
                                                             const DocumentRenderData &data) const
{
    DocumentExportResult result;
    result.documentType = snapshot.documentType;
    result.outputPath = snapshot.pdfPath;
    result.exportedPageCount = data.pages.pages.size();

    if (!snapshot.available) {
        result.errorMessage = "Документ недоступен для export";
        return result;
    }

    if (snapshot.pdfPath.trimmed().isEmpty()) {
        result.errorMessage = "Не подготовлен путь PDF для export";
        return result;
    }

    result.success = true;
    return result;
}
