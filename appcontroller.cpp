#include "appcontroller.h"

AppController::AppController(QObject *parent)
    : QObject(parent)
{
    connect(&m_facade, &ProjectFacade::currentXmlPathChanged, this, &AppController::currentXmlPathChanged);
    connect(&m_facade, &ProjectFacade::currentProjectChanged, this, &AppController::currentProjectCodeChanged);
    connect(&m_facade, &ProjectFacade::statusMessageChanged, this, &AppController::statusMessageChanged);
    connect(&m_facade, &ProjectFacade::lastErrorChanged, this, &AppController::lastErrorChanged);
    connect(&m_facade, &ProjectFacade::importedFilesChanged, this, &AppController::importedFilesChanged);
    connect(&m_facade, &ProjectFacade::stampDataChanged, this, &AppController::stampDataChanged);
    connect(&m_facade, &ProjectFacade::generationArtifactsChanged, this, &AppController::generationArtifactsChanged);
}

QString AppController::currentXmlPath() const
{
    return m_facade.currentXmlPath();
}

QString AppController::currentProjectCode() const
{
    return m_facade.currentProjectCode();
}

QString AppController::statusMessage() const
{
    return m_facade.statusMessage();
}

QString AppController::lastError() const
{
    return m_facade.lastError();
}

QStringList AppController::importedFiles() const
{
    return m_facade.importedFiles();
}

QString AppController::projectName() const
{
    return m_facade.projectName();
}

QString AppController::decimalNumber() const
{
    return m_facade.decimalNumber();
}

QString AppController::developer() const
{
    return m_facade.developer();
}

QString AppController::checker() const
{
    return m_facade.checker();
}

QString AppController::approver() const
{
    return m_facade.approver();
}

QString AppController::normControl() const
{
    return m_facade.normControl();
}

QString AppController::departmentHead() const
{
    return m_facade.departmentHead();
}

QString AppController::departmentHeadAlt() const
{
    return m_facade.departmentHeadAlt();
}

QString AppController::generationOutputDirectory() const
{
    return m_facade.generationOutputDirectory();
}

QString AppController::generationDocumentationDirectory() const
{
    return m_facade.generationDocumentationDirectory();
}

QString AppController::peOutputPath() const
{
    return m_facade.peOutputPath();
}

QString AppController::spOutputPath() const
{
    return m_facade.spOutputPath();
}

QString AppController::vpOutputPath() const
{
    return m_facade.vpOutputPath();
}

QString AppController::groupedVpOutputPath() const
{
    return m_facade.groupedVpOutputPath();
}

QString AppController::pePdfOutputPath() const
{
    return m_facade.pePdfOutputPath();
}

QString AppController::spPdfOutputPath() const
{
    return m_facade.spPdfOutputPath();
}

QString AppController::vpPdfOutputPath() const
{
    return m_facade.vpPdfOutputPath();
}

QString AppController::groupedVpPdfOutputPath() const
{
    return m_facade.groupedVpPdfOutputPath();
}

bool AppController::loadProject(const QString &xmlPath)
{
    return m_facade.loadProject(xmlPath);
}

bool AppController::importProjects(const QStringList &xmlPaths)
{
    return m_facade.importProjects(xmlPaths);
}

void AppController::setProjectName(const QString &value)
{
    m_facade.setProjectName(value);
}

void AppController::setDecimalNumber(const QString &value)
{
    m_facade.setDecimalNumber(value);
}

void AppController::setDeveloper(const QString &value)
{
    m_facade.setDeveloper(value);
}

void AppController::setChecker(const QString &value)
{
    m_facade.setChecker(value);
}

void AppController::setApprover(const QString &value)
{
    m_facade.setApprover(value);
}

void AppController::setNormControl(const QString &value)
{
    m_facade.setNormControl(value);
}

void AppController::setDepartmentHead(const QString &value)
{
    m_facade.setDepartmentHead(value);
}

void AppController::setDepartmentHeadAlt(const QString &value)
{
    m_facade.setDepartmentHeadAlt(value);
}

bool AppController::prepareGenerationArtifacts(const QString &baseDirectory)
{
    return m_facade.prepareGenerationArtifacts(baseDirectory);
}

bool AppController::generatePeDocument()
{
    return m_facade.generatePeDocument();
}

bool AppController::generateSpDocument()
{
    return m_facade.generateSpDocument();
}

bool AppController::generateVpDocument()
{
    return m_facade.generateVpDocument();
}

bool AppController::generateGroupedVpDocument()
{
    return m_facade.generateGroupedVpDocument();
}

bool AppController::hasPreviewDocument(const QString &documentType) const
{
    return m_facade.hasPreviewDocument(documentType);
}

QString AppController::previewDocumentPath(const QString &documentType) const
{
    return m_facade.previewDocumentPath(documentType);
}

QString AppController::previewDocumentPdfPath(const QString &documentType) const
{
    return m_facade.previewDocumentPdfPath(documentType);
}

int AppController::previewDocumentPageCount(const QString &documentType) const
{
    return m_facade.previewDocumentPageCount(documentType);
}

QString AppController::previewDocumentTitle(const QString &documentType) const
{
    return m_facade.previewDocumentTitle(documentType);
}

QString AppController::previewPageFormat(const QString &documentType) const
{
    return m_facade.previewPageFormat(documentType);
}

QString AppController::previewStampRoleSummary(const QString &documentType) const
{
    return m_facade.previewStampRoleSummary(documentType);
}

bool AppController::previewPainterReady(const QString &documentType) const
{
    return m_facade.previewPainterReady(documentType);
}

bool AppController::exportPreviewDocumentPdf(const QString &documentType)
{
    return m_facade.exportPreviewDocumentPdf(documentType);
}
