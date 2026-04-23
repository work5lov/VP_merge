#include "backend/projectfacade.h"

#include <QDir>
#include <QFileInfo>
#include <QString>

namespace {
QString firstNonEmpty(const QStringList &values)
{
    for (const QString &value : values) {
        if (!value.trimmed().isEmpty()) {
            return value.trimmed();
        }
    }
    return QString();
}
}

ProjectFacade::ProjectFacade(QObject *parent)
    : QObject(parent)
{
}

QString ProjectFacade::currentXmlPath() const
{
    return m_currentProject.sourceXmlPath;
}

QString ProjectFacade::currentProjectCode() const
{
    return m_currentProject.projectCode;
}

QString ProjectFacade::statusMessage() const
{
    return m_statusMessage;
}

QString ProjectFacade::lastError() const
{
    return m_lastError;
}

QStringList ProjectFacade::importedFiles() const
{
    return m_importSummary.xmlFiles;
}

QString ProjectFacade::projectName() const
{
    return m_currentProject.stampFields.projectName;
}

QString ProjectFacade::decimalNumber() const
{
    return m_currentProject.stampFields.decimalNumber;
}

QString ProjectFacade::developer() const
{
    return m_currentProject.stampFields.developer;
}

QString ProjectFacade::checker() const
{
    return m_currentProject.stampFields.checker;
}

QString ProjectFacade::approver() const
{
    return m_currentProject.stampFields.approver;
}

QString ProjectFacade::normControl() const
{
    return m_currentProject.stampFields.normControl;
}

QString ProjectFacade::departmentHead() const
{
    return m_currentProject.stampFields.departmentHead;
}

QString ProjectFacade::departmentHeadAlt() const
{
    return m_currentProject.stampFields.departmentHeadAlt;
}

QString ProjectFacade::generationOutputDirectory() const
{
    return m_generationArtifacts.outputDirectory;
}

QString ProjectFacade::generationDocumentationDirectory() const
{
    return m_generationArtifacts.documentationDirectory;
}

QString ProjectFacade::peOutputPath() const
{
    return m_generationArtifacts.pePath;
}

QString ProjectFacade::spOutputPath() const
{
    return m_generationArtifacts.spPath;
}

QString ProjectFacade::vpOutputPath() const
{
    return m_generationArtifacts.vpPath;
}

QString ProjectFacade::groupedVpOutputPath() const
{
    return m_generationArtifacts.groupedVpPath;
}

QString ProjectFacade::pePdfOutputPath() const
{
    return m_generationArtifacts.pePdfPath;
}

QString ProjectFacade::spPdfOutputPath() const
{
    return m_generationArtifacts.spPdfPath;
}

QString ProjectFacade::vpPdfOutputPath() const
{
    return m_generationArtifacts.vpPdfPath;
}

QString ProjectFacade::groupedVpPdfOutputPath() const
{
    return m_generationArtifacts.groupedVpPdfPath;
}

bool ProjectFacade::loadProject(const QString &xmlPath)
{
    if (xmlPath.trimmed().isEmpty()) {
        setLastError(QStringLiteral("Не указан путь к XML файлу"));
        return false;
    }

    m_currentProject = m_parser.parseProject(xmlPath);
    if (m_currentProject.sourceXmlPath.isEmpty()) {
        setLastError(QStringLiteral("Не удалось загрузить проект"));
        return false;
    }

    m_currentProject.stampFields.decimalNumber = firstNonEmpty({
        m_currentProject.projectCode,
        m_currentProject.stamps.pe.value(QStringLiteral("Децимальный номер")),
        m_currentProject.stamps.sp.value(QStringLiteral("Децимальный номер")),
        m_currentProject.stamps.vp.value(QStringLiteral("Децимальный номер"))
    });
    m_currentProject.stampFields.projectName = firstNonEmpty({
        m_currentProject.stamps.pe.value(QStringLiteral("Наименование проекта")),
        m_currentProject.stamps.sp.value(QStringLiteral("Наименование проекта")),
        m_currentProject.stamps.vp.value(QStringLiteral("Наименование проекта"))
    });
    m_currentProject.stampFields.developer = firstNonEmpty({
        m_currentProject.stamps.pe.value(QStringLiteral("Разработал")),
        m_currentProject.stamps.sp.value(QStringLiteral("Разработал")),
        m_currentProject.stamps.vp.value(QStringLiteral("Разработал"))
    });
    m_currentProject.stampFields.checker = firstNonEmpty({
        m_currentProject.stamps.pe.value(QStringLiteral("Проверил")),
        m_currentProject.stamps.sp.value(QStringLiteral("Проверил")),
        m_currentProject.stamps.vp.value(QStringLiteral("Проверил"))
    });
    m_currentProject.stampFields.approver = firstNonEmpty({
        m_currentProject.stamps.pe.value(QStringLiteral("Утвердил")),
        m_currentProject.stamps.sp.value(QStringLiteral("Утвердил")),
        m_currentProject.stamps.vp.value(QStringLiteral("Утвердил"))
    });
    m_currentProject.stampFields.normControl = firstNonEmpty({
        m_currentProject.stamps.pe.value(QStringLiteral("Нормоконтроль")),
        m_currentProject.stamps.sp.value(QStringLiteral("Нормоконтроль")),
        m_currentProject.stamps.vp.value(QStringLiteral("Нормоконтроль"))
    });
    m_currentProject.stampFields.departmentHead = firstNonEmpty({
        m_currentProject.stamps.pe.value(QStringLiteral("Начальник отдела")),
        m_currentProject.stamps.sp.value(QStringLiteral("Начальник отдела")),
        m_currentProject.stamps.vp.value(QStringLiteral("Начальник отдела"))
    });
    m_currentProject.stampFields.departmentHeadAlt = firstNonEmpty({
        m_currentProject.stamps.pe.value(QStringLiteral("Начальник отдела ( альтернативное название параметра)")),
        m_currentProject.stamps.sp.value(QStringLiteral("Начальник отдела ( альтернативное название параметра)")),
        m_currentProject.stamps.vp.value(QStringLiteral("Начальник отдела ( альтернативное название параметра)"))
    });

    m_generationArtifacts = GenerationArtifacts();
    syncStampMaps();

    emit currentXmlPathChanged();
    emit currentProjectChanged();
    emit stampDataChanged();
    emit generationArtifactsChanged();
    setLastError(QString());
    setStatusMessage(QStringLiteral("Проект загружен: %1").arg(m_currentProject.projectCode));
    return true;
}

bool ProjectFacade::importProjects(const QStringList &xmlPaths)
{
    QStringList normalized;
    QStringList codes;

    for (const QString &path : xmlPaths) {
        if (path.trimmed().isEmpty()) {
            continue;
        }
        normalized.append(path);
        codes.append(m_parser.extractProjectCode(path));
    }

    if (normalized.isEmpty()) {
        setLastError(QStringLiteral("Список XML файлов пуст"));
        return false;
    }

    m_importSummary.xmlFiles = normalized;
    m_importSummary.projectCodes = codes;
    emit importedFilesChanged();
    setLastError(QString());
    setStatusMessage(QStringLiteral("Импортировано файлов: %1").arg(normalized.size()));
    return true;
}

void ProjectFacade::setProjectName(const QString &value)
{
    const QString normalized = value.trimmed();
    if (m_currentProject.stampFields.projectName == normalized) {
        return;
    }
    m_currentProject.stampFields.projectName = normalized;
    syncStampMaps();
    emit stampDataChanged();
}

void ProjectFacade::setDecimalNumber(const QString &value)
{
    const QString normalized = value.trimmed();
    if (m_currentProject.stampFields.decimalNumber == normalized) {
        return;
    }
    m_currentProject.stampFields.decimalNumber = normalized;
    if (!normalized.isEmpty()) {
        m_currentProject.projectCode = normalized;
        emit currentProjectChanged();
    }
    syncStampMaps();
    emit stampDataChanged();
}

void ProjectFacade::setDeveloper(const QString &value)
{
    const QString normalized = value.trimmed();
    if (m_currentProject.stampFields.developer == normalized) {
        return;
    }
    m_currentProject.stampFields.developer = normalized;
    applyCommonStampValue(QStringLiteral("Разработал"), normalized);
    emit stampDataChanged();
}

void ProjectFacade::setChecker(const QString &value)
{
    const QString normalized = value.trimmed();
    if (m_currentProject.stampFields.checker == normalized) {
        return;
    }
    m_currentProject.stampFields.checker = normalized;
    applyCommonStampValue(QStringLiteral("Проверил"), normalized);
    emit stampDataChanged();
}

void ProjectFacade::setApprover(const QString &value)
{
    const QString normalized = value.trimmed();
    if (m_currentProject.stampFields.approver == normalized) {
        return;
    }
    m_currentProject.stampFields.approver = normalized;
    applyCommonStampValue(QStringLiteral("Утвердил"), normalized);
    emit stampDataChanged();
}

void ProjectFacade::setNormControl(const QString &value)
{
    const QString normalized = value.trimmed();
    if (m_currentProject.stampFields.normControl == normalized) {
        return;
    }
    m_currentProject.stampFields.normControl = normalized;
    applyCommonStampValue(QStringLiteral("Нормоконтроль"), normalized);
    emit stampDataChanged();
}

void ProjectFacade::setDepartmentHead(const QString &value)
{
    const QString normalized = value.trimmed();
    if (m_currentProject.stampFields.departmentHead == normalized) {
        return;
    }
    m_currentProject.stampFields.departmentHead = normalized;
    applyCommonStampValue(QStringLiteral("Начальник отдела"), normalized);
    emit stampDataChanged();
}

void ProjectFacade::setDepartmentHeadAlt(const QString &value)
{
    const QString normalized = value.trimmed();
    if (m_currentProject.stampFields.departmentHeadAlt == normalized) {
        return;
    }
    m_currentProject.stampFields.departmentHeadAlt = normalized;
    applyCommonStampValue(QStringLiteral("Начальник отдела ( альтернативное название параметра)"), normalized);
    emit stampDataChanged();
}

bool ProjectFacade::prepareGenerationArtifacts(const QString &baseDirectory)
{
    if (m_currentProject.sourceXmlPath.trimmed().isEmpty()) {
        setLastError(QStringLiteral("Сначала загрузите проект"));
        return false;
    }

    QString resolvedBaseDirectory = baseDirectory.trimmed();
    if (resolvedBaseDirectory.isEmpty()) {
        QFileInfo sourceInfo(m_currentProject.sourceXmlPath);
        resolvedBaseDirectory = sourceInfo.absolutePath();
    }

    if (resolvedBaseDirectory.isEmpty()) {
        setLastError(QStringLiteral("Не удалось определить директорию вывода"));
        return false;
    }

    QDir baseDir(resolvedBaseDirectory);
    if (!baseDir.exists()) {
        setLastError(QStringLiteral("Базовая директория не существует: %1").arg(resolvedBaseDirectory));
        return false;
    }

    const QString documentationFolderName = QStringLiteral("Документация");
    if (!baseDir.exists(documentationFolderName) && !baseDir.mkdir(documentationFolderName)) {
        setLastError(QStringLiteral("Не удалось создать каталог: %1").arg(baseDir.filePath(documentationFolderName)));
        return false;
    }

    const QString code = effectiveProjectCode();
    m_generationArtifacts.outputDirectory = baseDir.absolutePath();
    m_generationArtifacts.documentationDirectory = baseDir.filePath(documentationFolderName);
    m_generationArtifacts.pePath = baseDir.filePath(documentationFolderName + "/Перечень элементов " + code + "ПЭ3.xlsx");
    m_generationArtifacts.spPath = baseDir.filePath(documentationFolderName + "/Спецификация " + code + ".xlsx");
    m_generationArtifacts.vpPath = baseDir.filePath(documentationFolderName + "/Ведомость покупных " + code + "ВП.xlsx");
    m_generationArtifacts.groupedVpPath = baseDir.filePath(documentationFolderName + "/Групповая ведомость покупных " + code + "ВП.xlsx");
    m_generationArtifacts.pePdfPath = baseDir.filePath(documentationFolderName + "/Перечень элементов " + code + "ПЭ3.pdf");
    m_generationArtifacts.spPdfPath = baseDir.filePath(documentationFolderName + "/Спецификация " + code + ".pdf");
    m_generationArtifacts.vpPdfPath = baseDir.filePath(documentationFolderName + "/Ведомость покупных " + code + "ВП.pdf");
    m_generationArtifacts.groupedVpPdfPath = baseDir.filePath(documentationFolderName + "/Групповая ведомость покупных " + code + "ВП.pdf");
    m_generationArtifacts.peGenerated = false;
    m_generationArtifacts.spGenerated = false;
    m_generationArtifacts.vpGenerated = false;
    m_generationArtifacts.groupedVpGenerated = false;

    emit generationArtifactsChanged();
    setLastError(QString());
    setStatusMessage(QStringLiteral("Подготовлены пути генерации для проекта %1").arg(code));
    return true;
}

bool ProjectFacade::generatePeDocument()
{
    if (m_currentProject.sourceXmlPath.trimmed().isEmpty()) {
        setLastError(QStringLiteral("Сначала загрузите проект"));
        return false;
    }

    if (m_generationArtifacts.pePath.trimmed().isEmpty()) {
        setLastError(QStringLiteral("Сначала подготовьте пути генерации"));
        return false;
    }

    QString errorMessage;
    if (!m_peGeneratorService.generate(m_currentProject, m_generationArtifacts.pePath, &errorMessage)) {
        m_generationArtifacts.peGenerated = false;
        emit generationArtifactsChanged();
        setLastError(errorMessage.isEmpty() ? QStringLiteral("Не удалось сформировать ПЭ") : errorMessage);
        return false;
    }

    m_generationArtifacts.peGenerated = true;
    emit generationArtifactsChanged();
    setLastError(QString());
    setStatusMessage(QStringLiteral("Сформирован документ ПЭ: %1").arg(m_generationArtifacts.pePath));
    return true;
}

bool ProjectFacade::generateSpDocument()
{
    if (m_currentProject.sourceXmlPath.trimmed().isEmpty()) {
        setLastError(QStringLiteral("Сначала загрузите проект"));
        return false;
    }

    if (m_generationArtifacts.spPath.trimmed().isEmpty()) {
        setLastError(QStringLiteral("Сначала подготовьте пути генерации"));
        return false;
    }

    QString errorMessage;
    if (!m_spGeneratorService.generate(m_currentProject, m_generationArtifacts.spPath, &errorMessage)) {
        m_generationArtifacts.spGenerated = false;
        emit generationArtifactsChanged();
        setLastError(errorMessage.isEmpty() ? QStringLiteral("Не удалось сформировать СП") : errorMessage);
        return false;
    }

    m_generationArtifacts.spGenerated = true;
    emit generationArtifactsChanged();
    setLastError(QString());
    setStatusMessage(QStringLiteral("Сформирован документ СП: %1").arg(m_generationArtifacts.spPath));
    return true;
}

bool ProjectFacade::generateVpDocument()
{
    if (m_currentProject.sourceXmlPath.trimmed().isEmpty()) {
        setLastError(QStringLiteral("Сначала загрузите проект"));
        return false;
    }

    if (m_generationArtifacts.vpPath.trimmed().isEmpty()) {
        setLastError(QStringLiteral("Сначала подготовьте пути генерации"));
        return false;
    }

    QString errorMessage;
    if (!m_vpGeneratorService.generate(m_currentProject, m_generationArtifacts.vpPath, &errorMessage)) {
        m_generationArtifacts.vpGenerated = false;
        emit generationArtifactsChanged();
        setLastError(errorMessage.isEmpty() ? QStringLiteral("Не удалось сформировать ВП") : errorMessage);
        return false;
    }

    m_generationArtifacts.vpGenerated = true;
    emit generationArtifactsChanged();
    setLastError(QString());
    setStatusMessage(QStringLiteral("Сформирован документ ВП: %1").arg(m_generationArtifacts.vpPath));
    return true;
}

bool ProjectFacade::generateGroupedVpDocument()
{
    if (m_importSummary.xmlFiles.isEmpty()) {
        setLastError(QStringLiteral("Сначала импортируйте XML файлы для групповой ВП"));
        return false;
    }

    if (m_generationArtifacts.groupedVpPath.trimmed().isEmpty()) {
        setLastError(QStringLiteral("Сначала подготовьте пути генерации"));
        return false;
    }

    QString errorMessage;
    if (!m_groupedVpGeneratorService.generate(m_importSummary,
                                              m_currentProject.stamps,
                                              m_currentProject.stampFields,
                                              m_generationArtifacts.groupedVpPath,
                                              m_parser,
                                              &errorMessage)) {
        m_generationArtifacts.groupedVpGenerated = false;
        emit generationArtifactsChanged();
        setLastError(errorMessage.isEmpty() ? QStringLiteral("Не удалось сформировать групповую ВП") : errorMessage);
        return false;
    }

    m_generationArtifacts.groupedVpGenerated = true;
    emit generationArtifactsChanged();
    setLastError(QString());
    setStatusMessage(QStringLiteral("Сформирована групповая ВП: %1").arg(m_generationArtifacts.groupedVpPath));
    return true;
}

bool ProjectFacade::hasPreviewDocument(const QString &documentType) const
{
    const QString normalized = documentType.trimmed().toUpper();
    return buildRenderData(normalized == QStringLiteral("SP") ? DocumentKind::Sp
                           : normalized == QStringLiteral("VP") ? DocumentKind::Vp
                           : normalized == QStringLiteral("GVP") ? DocumentKind::GroupedVp
                           : DocumentKind::Pe).available;
}

QString ProjectFacade::previewDocumentPath(const QString &documentType) const
{
    const QString normalized = documentType.trimmed().toUpper();
    if (normalized == QStringLiteral("SP")) {
        return m_generationArtifacts.spPath;
    }
    if (normalized == QStringLiteral("VP")) {
        return m_generationArtifacts.vpPath;
    }
    if (normalized == QStringLiteral("GVP")) {
        return m_generationArtifacts.groupedVpPath;
    }
    return m_generationArtifacts.pePath;
}

QString ProjectFacade::previewDocumentPdfPath(const QString &documentType) const
{
    const QString normalized = documentType.trimmed().toUpper();
    if (normalized == QStringLiteral("SP")) {
        return m_generationArtifacts.spPdfPath;
    }
    if (normalized == QStringLiteral("VP")) {
        return m_generationArtifacts.vpPdfPath;
    }
    if (normalized == QStringLiteral("GVP")) {
        return m_generationArtifacts.groupedVpPdfPath;
    }
    return m_generationArtifacts.pePdfPath;
}

int ProjectFacade::previewDocumentPageCount(const QString &documentType) const
{
    return previewSnapshot(documentType).pageCount;
}

QString ProjectFacade::previewDocumentTitle(const QString &documentType) const
{
    return previewSnapshot(documentType).documentTitle;
}

QString ProjectFacade::previewPageFormat(const QString &documentType) const
{
    return previewSnapshot(documentType).pageFormat;
}

QString ProjectFacade::previewStampRoleSummary(const QString &documentType) const
{
    return previewSnapshot(documentType).stampRoleSummary;
}

bool ProjectFacade::previewPainterReady(const QString &documentType) const
{
    return previewSnapshot(documentType).painterReady;
}

bool ProjectFacade::exportPreviewDocumentPdf(const QString &documentType)
{
    const QString normalized = documentType.trimmed().toUpper();
    const DocumentKind kind = normalized == QStringLiteral("SP") ? DocumentKind::Sp
                              : normalized == QStringLiteral("VP") ? DocumentKind::Vp
                              : normalized == QStringLiteral("GVP") ? DocumentKind::GroupedVp
                              : DocumentKind::Pe;

    const DocumentRenderSnapshot snapshot = buildRenderSnapshot(kind);
    const DocumentRenderData data = buildRenderData(kind);
    m_lastDocumentExportResult = m_headlessDocumentExportService.exportPdf(snapshot, data);

    if (!m_lastDocumentExportResult.success) {
        setLastError(m_lastDocumentExportResult.errorMessage);
        setStatusMessage(QStringLiteral("Не удалось подготовить headless export PDF для %1").arg(snapshot.documentTitle));
        return false;
    }

    setLastError(QString());
    setStatusMessage(QStringLiteral("Headless export PDF подготовлен для %1: %2")
                     .arg(snapshot.documentTitle, m_lastDocumentExportResult.outputPath));
    return true;
}

const ProjectData &ProjectFacade::currentProject() const
{
    return m_currentProject;
}

ImportSummary ProjectFacade::importSummary() const
{
    return m_importSummary;
}

GenerationArtifacts ProjectFacade::generationArtifacts() const
{
    return m_generationArtifacts;
}

DocumentPreviewState ProjectFacade::previewState() const
{
    DocumentPreviewState state;
    state.documents.append(buildRenderData(DocumentKind::Pe));
    state.documents.append(buildRenderData(DocumentKind::Sp));
    state.documents.append(buildRenderData(DocumentKind::Vp));
    state.documents.append(buildRenderData(DocumentKind::GroupedVp));

    for (int index = 0; index < state.documents.size(); ++index) {
        if (state.documents.at(index).available) {
            state.currentIndex = index;
            break;
        }
    }

    return state;
}

DocumentRenderSnapshot ProjectFacade::previewSnapshot(const QString &documentType) const
{
    const QString normalized = documentType.trimmed().toUpper();
    return buildRenderSnapshot(normalized == QStringLiteral("SP") ? DocumentKind::Sp
                               : normalized == QStringLiteral("VP") ? DocumentKind::Vp
                               : normalized == QStringLiteral("GVP") ? DocumentKind::GroupedVp
                               : DocumentKind::Pe);
}

DocumentExportResult ProjectFacade::lastDocumentExportResult() const
{
    return m_lastDocumentExportResult;
}

void ProjectFacade::setStatusMessage(const QString &message)
{
    if (m_statusMessage == message) {
        return;
    }
    m_statusMessage = message;
    emit statusMessageChanged();
}

void ProjectFacade::setLastError(const QString &message)
{
    if (m_lastError == message) {
        return;
    }
    m_lastError = message;
    emit lastErrorChanged();
}

void ProjectFacade::syncStampMaps()
{
    applyCommonStampValue(QStringLiteral("Децимальный номер"), m_currentProject.stampFields.decimalNumber);
    applyCommonStampValue(QStringLiteral("Наименование проекта"), m_currentProject.stampFields.projectName);
    applyCommonStampValue(QStringLiteral("Разработал"), m_currentProject.stampFields.developer);
    applyCommonStampValue(QStringLiteral("Проверил"), m_currentProject.stampFields.checker);
    applyCommonStampValue(QStringLiteral("Утвердил"), m_currentProject.stampFields.approver);
    applyCommonStampValue(QStringLiteral("Нормоконтроль"), m_currentProject.stampFields.normControl);
    applyCommonStampValue(QStringLiteral("Начальник отдела"), m_currentProject.stampFields.departmentHead);
    applyCommonStampValue(QStringLiteral("Начальник отдела ( альтернативное название параметра)"), m_currentProject.stampFields.departmentHeadAlt);
}

void ProjectFacade::applyCommonStampValue(const QString &key, const QString &value)
{
    m_currentProject.stamps.pe.insert(key, value);
    m_currentProject.stamps.sp.insert(key, value);
    m_currentProject.stamps.vp.insert(key, value);
}

QString ProjectFacade::effectiveProjectCode() const
{
    return firstNonEmpty({
        m_currentProject.stampFields.decimalNumber,
        m_currentProject.projectCode,
        QStringLiteral("project")
    });
}

DocumentRenderData ProjectFacade::buildRenderData(DocumentKind kind) const
{
    DocumentRenderData data;
    data.kind = kind;
    data.documentCode = effectiveProjectCode();
    data.outputPath = previewDocumentPath(documentTypeKey(kind));
    data.stamp = m_currentProject.stamps.pe;

    switch (kind) {
    case DocumentKind::Pe:
        data.stamp = m_currentProject.stamps.pe;
        data.pages = m_parser.paginatePeLines(m_currentProject.peLines);
        break;
    case DocumentKind::Sp:
        data.stamp = m_currentProject.stamps.sp;
        data.pages = m_parser.paginateSpLines(m_currentProject.spLines);
        break;
    case DocumentKind::Vp:
        data.stamp = m_currentProject.stamps.vp;
        data.pages = m_parser.paginateVpLines(m_currentProject.vpLines);
        break;
    case DocumentKind::GroupedVp:
        data.stamp = m_currentProject.stamps.vp;
        break;
    }

    data.available = !data.pages.pages.isEmpty() || kind == DocumentKind::GroupedVp;
    return data;
}

QString ProjectFacade::documentTypeKey(DocumentKind kind) const
{
    switch (kind) {
    case DocumentKind::Pe:
        return QStringLiteral("PE");
    case DocumentKind::Sp:
        return QStringLiteral("SP");
    case DocumentKind::Vp:
        return QStringLiteral("VP");
    case DocumentKind::GroupedVp:
        return QStringLiteral("GVP");
    }

    return QString();
}

QString ProjectFacade::pdfOutputPath(DocumentKind kind) const
{
    switch (kind) {
    case DocumentKind::Pe:
        return m_generationArtifacts.pePdfPath;
    case DocumentKind::Sp:
        return m_generationArtifacts.spPdfPath;
    case DocumentKind::Vp:
        return m_generationArtifacts.vpPdfPath;
    case DocumentKind::GroupedVp:
        return m_generationArtifacts.groupedVpPdfPath;
    }

    return QString();
}

DocumentRenderSnapshot ProjectFacade::buildRenderSnapshot(DocumentKind kind) const
{
    const DocumentRenderData data = buildRenderData(kind);

    DocumentRenderSnapshot snapshot;
    snapshot.documentType = documentTypeKey(kind);
    snapshot.documentTitle = documentTitle(kind);
    snapshot.documentCode = data.documentCode;
    snapshot.outputPath = data.outputPath;
    snapshot.pdfPath = pdfOutputPath(kind);
    snapshot.pageFormat = pageFormat(kind);
    snapshot.stampRoleSummary = stampRoleSummary(data.stamp);
    snapshot.pageCount = data.pages.pages.size();
    snapshot.available = data.available;
    snapshot.painterReady = data.available && kind != DocumentKind::GroupedVp;
    return snapshot;
}

QString ProjectFacade::documentTitle(DocumentKind kind) const
{
    switch (kind) {
    case DocumentKind::Pe:
        return QStringLiteral("Перечень элементов");
    case DocumentKind::Sp:
        return QStringLiteral("Спецификация");
    case DocumentKind::Vp:
        return QStringLiteral("Ведомость покупных");
    case DocumentKind::GroupedVp:
        return QStringLiteral("Групповая ведомость покупных");
    }

    return QString();
}

QString ProjectFacade::pageFormat(DocumentKind kind) const
{
    switch (kind) {
    case DocumentKind::Pe:
    case DocumentKind::Sp:
        return QStringLiteral("A4 portrait");
    case DocumentKind::Vp:
    case DocumentKind::GroupedVp:
        return QStringLiteral("A3 landscape");
    }

    return QString();
}

QString ProjectFacade::stampRoleSummary(const QMap<QString, QString> &stamp) const
{
    return QStringLiteral("%1 | %2 | %3")
        .arg(stamp.value(QStringLiteral("Разработал"), QStringLiteral("—")))
        .arg(stamp.value(QStringLiteral("Проверил"), QStringLiteral("—")))
        .arg(stamp.value(QStringLiteral("Утвердил"), QStringLiteral("—")));
}
