import QtQuick
import QtQuick.Controls

Window {
    id: root
    width: 480
    height: 900
    visible: true
    title: "Park Out"
    color: "#2c3e50"

    property real boardSize: Math.min(width, 520) - 16

    Connections {
        target: gameCtrl
        function onShowNotification(message) {
            notificationText.text = message
            notificationPopup.opacity = 1.0
            notificationTimer.restart()
        }
    }

    Column {
        spacing: 12
        anchors.centerIn: parent
        width: root.boardSize

        Text {
            text: "Park Out"
            color: "white"
            font.pixelSize: 24
            anchors.horizontalCenter: parent.horizontalCenter
        }

        // Game board 
        Rectangle {
            width: root.boardSize; 
            height: root.boardSize
            color: "#34495e"; 
            border.color: "#ecf0f1"; 
            border.width: 2
            clip: true; 
            radius: 8
            anchors.horizontalCenter: parent.horizontalCenterer

            
            Grid {
                anchors.fill: parent
                rows: gameCtrl.rows
                columns: gameCtrl.cols
                Repeater {
                    model: gameCtrl.rows * gameCtrl.cols
                    Rectangle {
                        width: root.boardSize / gameCtrl.cols
                        height: root.boardSize / gameCtrl.rows
                        color: "transparent"
                        border.color: "#2c3e50"
                        border.width: 1
                    }
                }
            }

            Repeater {
                model: gameCtrl.buses
                Rectangle {
                    property real cellSize: root.boardSize / gameCtrl.cols
                    property bool isHoriz: modelData.direction === "l" || modelData.direction === "r"
                    property int busLen: modelData.capacity / 2

                    // Posicionamento dinâmico baseado no estado atual da matriz C++
                    x: modelData.col * cellSize + 2
                    y: modelData.row * cellSize + 2

                    // Dimensionamento com base no tamanho calculado pelo Board no Passo 4
                    width: (modelData.direction === "r" || modelData.direction === "l") ? (modelData.length * cellSize) : cellSize
                    height: (modelData.direction === "d" || modelData.direction === "u") ? (modelData.length * cellSize) : cellSize

                    color: modelData.color
                    radius: 8
                    border.color: "#1a1a1a"
                    border.width: 2

                    Behavior on x { NumberAnimation { duration: 150; easing.type: Easing.OutQuad } }
                    Behavior on y { NumberAnimation { duration: 150; easing.type: Easing.OutQuad } }

                    // Texto com a capacidade do autocarro
                    Text {
                        anchors.centerIn: parent
                        text: modelData.capacity
                        color: "white"; 
                        font.bold: true; 
                        font.pixelSize: 12
                    }

                    MouseArea {
                        anchors.fill: parent // Preenche todo o retângulo do autocarro
                        cursorShape: Qt.PointingHandCursor // Transforma o cursor num ponteiro clicável

                        onClicked: {

                            // Invoca o método C++ passando o 'index' automático deste delegate
                            gameCtrl.handleBusClick(index);
                        }
                    }
                }
            }
        }
    }
}