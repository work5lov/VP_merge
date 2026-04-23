import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import com.vpmerge 1.0

ApplicationWindow {
    id: root
    visible: true
    width: 1180
    height: 900
    title: qsTr("VP Merge - Hybrid Migration")

    AppController {
        id: controller
    }

    property string xmlInput: "boas.xml"
    property string outputDirectoryInput: ""
    property int previewDocumentIndex: 0
    property var previewDocumentTypes: ["PE", "SP", "VP", "GVP"]
    property var previewDocumentTitles: [qsTr("Перечень элементов"), qsTr("Спецификация"), qsTr("Ведомость покупных"), qsTr("Групповая ведомость покупных")]
    property string selectedPreviewDocumentType: previewDocumentTypes[previewDocumentIndex]

    ScrollView {
        anchors.fill: parent
        anchors.margins: 20
        clip: true

        ColumnLayout {
            width: root.width - 56
            spacing: 16

            Label {
                text: qsTr("Четвёртая итерация миграции: preview-метаданные и выбор документа через facade")
                font.pixelSize: 22
                Layout.fillWidth: true
                wrapMode: Text.Wrap
            }

            Frame {
                Layout.fillWidth: true

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 12

                    Label {
                        text: qsTr("Загрузка XML проекта")
                        font.bold: true
                    }

                    RowLayout {
                        Layout.fillWidth: true

                        TextField {
                            id: xmlPathField
                            Layout.fillWidth: true
                            text: root.xmlInput
                            placeholderText: qsTr("Введите относительный или абсолютный путь к XML")
                            onTextChanged: root.xmlInput = text
                        }

                        Button {
                            text: qsTr("Загрузить проект")
                            onClicked: controller.loadProject(xmlPathField.text)
                        }

                        Button {
                            text: qsTr("Импортировать в список")
                            onClicked: controller.importProjects([xmlPathField.text])
                        }
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 24

                        Label {
                            text: qsTr("Текущий XML: %1").arg(controller.currentXmlPath.length > 0 ? controller.currentXmlPath : "—")
                            Layout.fillWidth: true
                            wrapMode: Text.WrapAnywhere
                        }

                        Label {
                            text: qsTr("Код проекта: %1").arg(controller.currentProjectCode.length > 0 ? controller.currentProjectCode : "—")
                        }
                    }
                }
            }

            Frame {
                Layout.fillWidth: true

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 12

                    Label {
                        text: qsTr("Параметры проекта")
                        font.bold: true
                    }

                    GridLayout {
                        Layout.fillWidth: true
                        columns: 2
                        columnSpacing: 16
                        rowSpacing: 10

                        Label {
                            text: qsTr("Децимальный номер")
                        }

                        TextField {
                            Layout.fillWidth: true
                            text: controller.decimalNumber
                            placeholderText: qsTr("Например, АБВГ.123456.789")
                            onEditingFinished: controller.setDecimalNumber(text)
                        }

                        Label {
                            text: qsTr("Наименование проекта")
                        }

                        TextField {
                            Layout.fillWidth: true
                            text: controller.projectName
                            placeholderText: qsTr("Введите наименование проекта")
                            onEditingFinished: controller.setProjectName(text)
                        }
                    }
                }
            }

            Frame {
                Layout.fillWidth: true

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 12

                    Label {
                        text: qsTr("Данные штампа")
                        font.bold: true
                    }

                    GridLayout {
                        Layout.fillWidth: true
                        columns: 2
                        columnSpacing: 16
                        rowSpacing: 10

                        Label { text: qsTr("Разработал") }
                        TextField {
                            Layout.fillWidth: true
                            text: controller.developer
                            onEditingFinished: controller.setDeveloper(text)
                        }

                        Label { text: qsTr("Проверил") }
                        TextField {
                            Layout.fillWidth: true
                            text: controller.checker
                            onEditingFinished: controller.setChecker(text)
                        }

                        Label { text: qsTr("Утвердил") }
                        TextField {
                            Layout.fillWidth: true
                            text: controller.approver
                            onEditingFinished: controller.setApprover(text)
                        }

                        Label { text: qsTr("Нормоконтроль") }
                        TextField {
                            Layout.fillWidth: true
                            text: controller.normControl
                            onEditingFinished: controller.setNormControl(text)
                        }

                        Label { text: qsTr("Начальник отдела") }
                        TextField {
                            Layout.fillWidth: true
                            text: controller.departmentHead
                            onEditingFinished: controller.setDepartmentHead(text)
                        }

                        Label { text: qsTr("Начальник отдела (альт. название)") }
                        TextField {
                            Layout.fillWidth: true
                            text: controller.departmentHeadAlt
                            onEditingFinished: controller.setDepartmentHeadAlt(text)
                        }
                    }
                }
            }

            Frame {
                Layout.fillWidth: true

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 12

                    Label {
                        text: qsTr("Подготовка генерации")
                        font.bold: true
                    }

                    TextField {
                        id: outputDirectoryField
                        Layout.fillWidth: true
                        text: root.outputDirectoryInput
                        placeholderText: qsTr("Базовая директория вывода. Оставьте пустой, чтобы взять папку XML")
                        onTextChanged: root.outputDirectoryInput = text
                    }

                    RowLayout {
                        Layout.fillWidth: true

                        Button {
                            text: qsTr("Подготовить каталог и пути документов")
                            onClicked: controller.prepareGenerationArtifacts(outputDirectoryField.text)
                        }

                        Button {
                            text: qsTr("Сформировать ПЭ")
                            onClicked: controller.generatePeDocument()
                        }

                        Button {
                            text: qsTr("Сформировать СП")
                            onClicked: controller.generateSpDocument()
                        }

                        Button {
                            text: qsTr("Сформировать ВП")
                            onClicked: controller.generateVpDocument()
                        }

                        Button {
                            text: qsTr("Сформировать групповую ВП")
                            onClicked: controller.generateGroupedVpDocument()
                        }
                    }

                    GridLayout {
                        Layout.fillWidth: true
                        columns: 2
                        columnSpacing: 16
                        rowSpacing: 8

                        Label { text: qsTr("Базовая директория") }
                        TextField {
                            Layout.fillWidth: true
                            readOnly: true
                            text: controller.generationOutputDirectory
                        }

                        Label { text: qsTr("Каталог документации") }
                        TextField {
                            Layout.fillWidth: true
                            readOnly: true
                            text: controller.generationDocumentationDirectory
                        }

                        Label { text: qsTr("Путь ПЭ") }
                        TextField {
                            Layout.fillWidth: true
                            readOnly: true
                            text: controller.peOutputPath
                        }

                        Label { text: qsTr("Путь СП") }
                        TextField {
                            Layout.fillWidth: true
                            readOnly: true
                            text: controller.spOutputPath
                        }

                        Label { text: qsTr("Путь ВП") }
                        TextField {
                            Layout.fillWidth: true
                            readOnly: true
                            text: controller.vpOutputPath
                        }

                        Label { text: qsTr("Путь групповой ВП") }
                        TextField {
                            Layout.fillWidth: true
                            readOnly: true
                            text: controller.groupedVpOutputPath
                        }

                        Label { text: qsTr("PDF ПЭ") }
                        TextField {
                            Layout.fillWidth: true
                            readOnly: true
                            text: controller.pePdfOutputPath
                        }

                        Label { text: qsTr("PDF СП") }
                        TextField {
                            Layout.fillWidth: true
                            readOnly: true
                            text: controller.spPdfOutputPath
                        }

                        Label { text: qsTr("PDF ВП") }
                        TextField {
                            Layout.fillWidth: true
                            readOnly: true
                            text: controller.vpPdfOutputPath
                        }

                        Label { text: qsTr("PDF групповой ВП") }
                        TextField {
                            Layout.fillWidth: true
                            readOnly: true
                            text: controller.groupedVpPdfOutputPath
                        }
                    }
                }
            }

            Frame {
                Layout.fillWidth: true

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 12

                    Label {
                        text: qsTr("Preview-метаданные документа")
                        font.bold: true
                    }

                    ComboBox {
                        id: previewDocumentSelector
                        Layout.fillWidth: true
                        model: root.previewDocumentTitles
                        currentIndex: root.previewDocumentIndex
                        onActivated: root.previewDocumentIndex = currentIndex
                    }

                    GridLayout {
                        Layout.fillWidth: true
                        columns: 2
                        columnSpacing: 16
                        rowSpacing: 8

                        Label { text: qsTr("Тип документа") }
                        TextField {
                            Layout.fillWidth: true
                            readOnly: true
                            text: root.selectedPreviewDocumentType
                        }

                        Label { text: qsTr("Название документа") }
                        TextField {
                            Layout.fillWidth: true
                            readOnly: true
                            text: controller.previewDocumentTitle(root.selectedPreviewDocumentType)
                        }

                        Label { text: qsTr("Доступен для preview") }
                        TextField {
                            Layout.fillWidth: true
                            readOnly: true
                            text: controller.hasPreviewDocument(root.selectedPreviewDocumentType) ? qsTr("Да") : qsTr("Нет")
                        }

                        Label { text: qsTr("Painter-ready") }
                        TextField {
                            Layout.fillWidth: true
                            readOnly: true
                            text: controller.previewPainterReady(root.selectedPreviewDocumentType) ? qsTr("Да") : qsTr("Нет")
                        }

                        Label { text: qsTr("Количество страниц") }
                        TextField {
                            Layout.fillWidth: true
                            readOnly: true
                            text: controller.previewDocumentPageCount(root.selectedPreviewDocumentType).toString()
                        }

                        Label { text: qsTr("Формат страниц") }
                        TextField {
                            Layout.fillWidth: true
                            readOnly: true
                            text: controller.previewPageFormat(root.selectedPreviewDocumentType)
                        }

                        Label { text: qsTr("Штамп: ключевые роли") }
                        TextField {
                            Layout.fillWidth: true
                            readOnly: true
                            text: controller.previewStampRoleSummary(root.selectedPreviewDocumentType)
                        }

                        Label { text: qsTr("Путь XLSX/Excel") }
                        TextField {
                            Layout.fillWidth: true
                            readOnly: true
                            text: controller.previewDocumentPath(root.selectedPreviewDocumentType)
                        }

                        Label { text: qsTr("Путь PDF") }
                        TextField {
                            Layout.fillWidth: true
                            readOnly: true
                            text: controller.previewDocumentPdfPath(root.selectedPreviewDocumentType)
                        }
                    }

                    Label {
                        Layout.fillWidth: true
                        wrapMode: Text.Wrap
                        color: "#455a64"
                        text: controller.hasPreviewDocument(root.selectedPreviewDocumentType)
                              ? qsTr("Метаданные preview подготовлены backend facade. Следующий этап — перенос реального рендера страниц/PDF.")
                              : qsTr("Для выбранного документа preview-данные пока недоступны. Сначала загрузите проект и подготовьте артефакты генерации.")
                    }
                }
            }

            Frame {
                Layout.fillWidth: true
                Layout.preferredHeight: 280

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 12

                    Label {
                        text: qsTr("Статус backend facade")
                        font.bold: true
                    }

                    Label {
                        Layout.fillWidth: true
                        visible: controller.statusMessage.length > 0
                        text: qsTr("Статус: %1").arg(controller.statusMessage)
                        color: "#1b5e20"
                        wrapMode: Text.Wrap
                    }

                    Label {
                        Layout.fillWidth: true
                        visible: controller.lastError.length > 0
                        text: qsTr("Ошибка: %1").arg(controller.lastError)
                        color: "#b71c1c"
                        wrapMode: Text.Wrap
                    }

                    Label {
                        text: qsTr("Импортированные XML")
                        font.bold: true
                    }

                    ListView {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        clip: true
                        model: controller.importedFiles

                        delegate: Rectangle {
                            width: ListView.view.width
                            height: pathText.implicitHeight + 12
                            color: index % 2 === 0 ? "#f5f5f5" : "#ffffff"

                            Text {
                                id: pathText
                                anchors.fill: parent
                                anchors.margins: 6
                                text: modelData
                                wrapMode: Text.WrapAnywhere
                            }
                        }
                    }
                }
            }
        }
    }
}
