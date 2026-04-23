#ifndef DOCUMENTRENDERDATA_H
#define DOCUMENTRENDERDATA_H

#include <QList>
#include <QMap>
#include <QString>

#include "Structures.h"

enum class DocumentKind
{
    Pe,
    Sp,
    Vp,
    GroupedVp
};

struct DocumentRenderData
{
    DocumentKind kind = DocumentKind::Pe;
    QString documentCode;
    QString outputPath;
    QMap<QString, QString> stamp;
    PageContainer pages;
    bool available = false;
};

struct DocumentRenderSnapshot
{
    QString documentType;
    QString documentTitle;
    QString documentCode;
    QString outputPath;
    QString pdfPath;
    QString pageFormat;
    QString stampRoleSummary;
    int pageCount = 0;
    bool available = false;
    bool painterReady = false;
};

struct DocumentPreviewState
{
    QList<DocumentRenderData> documents;
    int currentIndex = -1;

    bool isValidIndex(int index) const
    {
        return index >= 0 && index < documents.size();
    }
};

#endif // DOCUMENTRENDERDATA_H
