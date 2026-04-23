#ifndef PROJECTFACADE_H
#define PROJECTFACADE_H

#include <QObject>
#include <QString>
#include <QStringList>

#include "backend/documentexportresult.h"
#include "backend/documentrenderdata.h"
#include "backend/groupedvpgeneratorservice.h"
#include "backend/headlessdocumentexportservice.h"
#include "backend/pegeneratorservice.h"
#include "backend/projecttypes.h"
#include "backend/spgeneratorservice.h"
#include "backend/vpgeneratorservice.h"
#include "backend/xmlprojectparser.h"

class ProjectFacade : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString currentXmlPath READ currentXmlPath NOTIFY currentXmlPathChanged)
    Q_PROPERTY(QString currentProjectCode READ currentProjectCode NOTIFY currentProjectChanged)
    Q_PROPERTY(QString statusMessage READ statusMessage NOTIFY statusMessageChanged)
    Q_PROPERTY(QString lastError READ lastError NOTIFY lastErrorChanged)
    Q_PROPERTY(QStringList importedFiles READ importedFiles NOTIFY importedFilesChanged)
    Q_PROPERTY(QString projectName READ projectName NOTIFY stampDataChanged)
    Q_PROPERTY(QString decimalNumber READ decimalNumber NOTIFY stampDataChanged)
    Q_PROPERTY(QString developer READ developer NOTIFY stampDataChanged)
    Q_PROPERTY(QString checker READ checker NOTIFY stampDataChanged)
    Q_PROPERTY(QString approver READ approver NOTIFY stampDataChanged)
    Q_PROPERTY(QString normControl READ normControl NOTIFY stampDataChanged)
    Q_PROPERTY(QString departmentHead READ departmentHead NOTIFY stampDataChanged)
    Q_PROPERTY(QString departmentHeadAlt READ departmentHeadAlt NOTIFY stampDataChanged)
    Q_PROPERTY(QString generationOutputDirectory READ generationOutputDirectory NOTIFY generationArtifactsChanged)
    Q_PROPERTY(QString generationDocumentationDirectory READ generationDocumentationDirectory NOTIFY generationArtifactsChanged)
    Q_PROPERTY(QString peOutputPath READ peOutputPath NOTIFY generationArtifactsChanged)
    Q_PROPERTY(QString spOutputPath READ spOutputPath NOTIFY generationArtifactsChanged)
    Q_PROPERTY(QString vpOutputPath READ vpOutputPath NOTIFY generationArtifactsChanged)
    Q_PROPERTY(QString groupedVpOutputPath READ groupedVpOutputPath NOTIFY generationArtifactsChanged)
    Q_PROPERTY(QString pePdfOutputPath READ pePdfOutputPath NOTIFY generationArtifactsChanged)
    Q_PROPERTY(QString spPdfOutputPath READ spPdfOutputPath NOTIFY generationArtifactsChanged)
    Q_PROPERTY(QString vpPdfOutputPath READ vpPdfOutputPath NOTIFY generationArtifactsChanged)
    Q_PROPERTY(QString groupedVpPdfOutputPath READ groupedVpPdfOutputPath NOTIFY generationArtifactsChanged)

public:
    explicit ProjectFacade(QObject *parent = nullptr);

    QString currentXmlPath() const;
    QString currentProjectCode() const;
    QString statusMessage() const;
    QString lastError() const;
    QStringList importedFiles() const;
    QString projectName() const;
    QString decimalNumber() const;
    QString developer() const;
    QString checker() const;
    QString approver() const;
    QString normControl() const;
    QString departmentHead() const;
    QString departmentHeadAlt() const;
    QString generationOutputDirectory() const;
    QString generationDocumentationDirectory() const;
    QString peOutputPath() const;
    QString spOutputPath() const;
    QString vpOutputPath() const;
    QString groupedVpOutputPath() const;
    QString pePdfOutputPath() const;
    QString spPdfOutputPath() const;
    QString vpPdfOutputPath() const;
    QString groupedVpPdfOutputPath() const;

    Q_INVOKABLE bool loadProject(const QString &xmlPath);
    Q_INVOKABLE bool importProjects(const QStringList &xmlPaths);
    Q_INVOKABLE void setProjectName(const QString &value);
    Q_INVOKABLE void setDecimalNumber(const QString &value);
    Q_INVOKABLE void setDeveloper(const QString &value);
    Q_INVOKABLE void setChecker(const QString &value);
    Q_INVOKABLE void setApprover(const QString &value);
    Q_INVOKABLE void setNormControl(const QString &value);
    Q_INVOKABLE void setDepartmentHead(const QString &value);
    Q_INVOKABLE void setDepartmentHeadAlt(const QString &value);
    Q_INVOKABLE bool prepareGenerationArtifacts(const QString &baseDirectory);
    Q_INVOKABLE bool generatePeDocument();
    Q_INVOKABLE bool generateSpDocument();
    Q_INVOKABLE bool generateVpDocument();
    Q_INVOKABLE bool generateGroupedVpDocument();
    Q_INVOKABLE bool hasPreviewDocument(const QString &documentType) const;
    Q_INVOKABLE QString previewDocumentPath(const QString &documentType) const;
    Q_INVOKABLE QString previewDocumentPdfPath(const QString &documentType) const;
    Q_INVOKABLE int previewDocumentPageCount(const QString &documentType) const;
    Q_INVOKABLE QString previewDocumentTitle(const QString &documentType) const;
    Q_INVOKABLE QString previewPageFormat(const QString &documentType) const;
    Q_INVOKABLE QString previewStampRoleSummary(const QString &documentType) const;
    Q_INVOKABLE bool previewPainterReady(const QString &documentType) const;
    Q_INVOKABLE bool exportPreviewDocumentPdf(const QString &documentType);

    const ProjectData &currentProject() const;
    ImportSummary importSummary() const;
    GenerationArtifacts generationArtifacts() const;
    DocumentPreviewState previewState() const;
    DocumentRenderSnapshot previewSnapshot(const QString &documentType) const;
    DocumentExportResult lastDocumentExportResult() const;

signals:
    void currentXmlPathChanged();
    void currentProjectChanged();
    void statusMessageChanged();
    void lastErrorChanged();
    void importedFilesChanged();
    void stampDataChanged();
    void generationArtifactsChanged();

private:
    void setStatusMessage(const QString &message);
    void setLastError(const QString &message);
    void syncStampMaps();
    void applyCommonStampValue(const QString &key, const QString &value);
    QString effectiveProjectCode() const;
    DocumentRenderData buildRenderData(DocumentKind kind) const;
    QString documentTypeKey(DocumentKind kind) const;
    QString pdfOutputPath(DocumentKind kind) const;
    DocumentRenderSnapshot buildRenderSnapshot(DocumentKind kind) const;
    QString documentTitle(DocumentKind kind) const;
    QString pageFormat(DocumentKind kind) const;
    QString stampRoleSummary(const QMap<QString, QString> &stamp) const;

    XmlProjectParser m_parser;
    ProjectData m_currentProject;
    ImportSummary m_importSummary;
    GenerationArtifacts m_generationArtifacts;
    PeGeneratorService m_peGeneratorService;
    SpGeneratorService m_spGeneratorService;
    VpGeneratorService m_vpGeneratorService;
    GroupedVpGeneratorService m_groupedVpGeneratorService;
    HeadlessDocumentExportService m_headlessDocumentExportService;
    DocumentExportResult m_lastDocumentExportResult;
    QString m_statusMessage;
    QString m_lastError;
};

#endif // PROJECTFACADE_H
