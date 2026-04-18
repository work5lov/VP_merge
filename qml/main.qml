import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import com.vpmerge 1.0

ApplicationWindow {
    id: root
    visible: true
    width: 1024
    height: 768
    title: qsTr("VP Merge - QML Interface")

    // Экземпляр контроллера, содержащего бизнес-логику
    AppController {
        id: controller
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 10
        padding: 20

        Button {
            text: qsTr("Создать VP")
            onClicked: controller.createVP("example.xlsx")
        }
        Button {
            text: qsTr("Экспортировать в PDF")
            onClicked: controller.exportToPdf()
        }
        // Добавьте другие элементы UI по мере необходимости
    }
}
