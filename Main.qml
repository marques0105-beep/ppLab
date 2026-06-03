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

    Timer {
        id: notificationTimer
        interval: 2000
        onTriggered: notificationPopup.opacity = 0.0
    }

    Column {
        spacing: 12
        anchors.centerIn: parent
        width: root.boardSize

        // Notificação
        Rectangle {
            id: notificationPopup
            width: root.boardSize
            height: 35
            color: "#e67e22"
            radius: 5
            opacity: 0.0
            anchors.horizontalCenter: parent.horizontalCenter
            Behavior on opacity { NumberAnimation { duration: 250 } }
            Text {
                id: notificationText
                anchors.centerIn: parent
                text: ""
                color: "white"
                font.bold: true
            }
        }

        // Contador de movimentos
        Rectangle {
            width: root.boardSize
            height: 40
            color: "#16a085"
            radius: 8
            Text {
                anchors.centerIn: parent
                text: "Movimentos: " + gameCtrl.moveCount
                color: "white"
                font.bold: true
                font.pixelSize: 16
            }
        }

        // Plataformas de estacionamento
        Item {
            width: root.boardSize
            height: 80
            anchors.horizontalCenter: parent.horizontalCenter

            Row {
                id: slotsRow
                spacing: 6
                anchors.fill: parent

                Repeater {
                    model: gameCtrl.numSlots
                    Rectangle {
                        width: (root.boardSize - (slotsRow.spacing * (gameCtrl.numSlots - 1))) / gameCtrl.numSlots
                        height: parent.height - 10
                        color: "#34495e"
                        border.color: "#95a5a6"
                        border.width: 2
                        radius: 8
                        Text {
                            anchors.bottom: parent.bottom
                            anchors.bottomMargin: 4
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: "S-" + (index + 1)
                            color: "#95a5a6"
                            font.pixelSize: 10
                        }
                    }
                }
            }

            Repeater {
                model: gameCtrl.parkedBuses
                Rectangle {
                    property real slotW: (root.boardSize - (6 * (gameCtrl.numSlots - 1))) / gameCtrl.numSlots
                    width: slotW
                    height: 70
                    radius: 6
                    border.color: "#1a1a1a"
                    border.width: 2
                    color: modelData.color
                    x: modelData.slotIndex * (slotW + 6)
                    y: 0
                    Text {
                        anchors.centerIn: parent
                        text: modelData.currentPassengers + "/" + modelData.capacity
                        color: "white"
                        font.bold: true
                        font.pixelSize: 11
                    }
                }
            }
        }

        // Tabuleiro
        Rectangle {
            width: root.boardSize
            height: root.boardSize
            color: "#34495e"
            border.color: "#ecf0f1"
            border.width: 2
            clip: true
            radius: 8

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

                    x: modelData.col * cellSize + 2
                    y: modelData.row * cellSize + 2
                    width:  (isHoriz ? busLen * cellSize : cellSize) - 4
                    height: (!isHoriz ? busLen * cellSize : cellSize) - 4
                    color: modelData.color
                    radius: 8
                    border.color: "#1a1a1a"
                    border.width: 2

                    Behavior on x { NumberAnimation { duration: 150; easing.type: Easing.OutQuad } }
                    Behavior on y { NumberAnimation { duration: 150; easing.type: Easing.OutQuad } }

                    Text {
                        anchors.centerIn: parent
                        text: "0/" + modelData.capacity
                        color: "white"
                        font.bold: true
                        font.pixelSize: 12
                    }

                    MouseArea {
                        anchors.fill: parent
                        property int startX: 0
                        property int startY: 0
                        property int threshold: 15

                        onPressed: (mouse) => {
                            startX = mouse.x
                            startY = mouse.y
                        }
                        onReleased: (mouse) => {
                            let diffX = mouse.x - startX
                            let diffY = mouse.y - startY
                            if (Math.abs(diffX) > threshold || Math.abs(diffY) > threshold) {
                                if      (modelData.direction === "r" && diffX >  threshold) gameCtrl.handleBusClick(index)
                                else if (modelData.direction === "l" && diffX < -threshold) gameCtrl.handleBusClick(index)
                                else if (modelData.direction === "d" && diffY >  threshold) gameCtrl.handleBusClick(index)
                                else if (modelData.direction === "u" && diffY < -threshold) gameCtrl.handleBusClick(index)
                            } else {
                                gameCtrl.handleBusClick(index)
                            }
                        }
                    }
                }
            }
        }
    }
}