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

    // ========== MENU ==========
    Column {
        id: menuScreen
        anchors.centerIn: parent
        spacing: 24
        visible: gameCtrl.inMenu

        Text {
            text: "PARK OUT"
            font.pixelSize: 50
            color: "#1abc9c"
            font.bold: true
            anchors.horizontalCenter: parent.horizontalCenter
        }
        Text {
            text: "Escolha um Nível:"
            font.pixelSize: 18
            color: "#ecf0f1"
            anchors.horizontalCenter: parent.horizontalCenter
        }
        Repeater {
            model: [1, 2, 3]
            Rectangle {
                width: Math.min(root.width - 40, 300)
                height: 80
                color: "#34495e"
                border.color: "#1abc9c"
                border.width: 2
                radius: 10
                anchors.horizontalCenter: parent.horizontalCenter
                Text {
                    text: "Nível " + modelData
                    color: "white"
                    font.bold: true
                    font.pixelSize: 18
                    anchors.centerIn: parent
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        gameCtrl.loadLevel(modelData)
                    }
                }
            }
        }
    }

    // ========== JOGO ==========
    Flickable {
        id: gameFlickable
        anchors.fill: parent
        visible: !gameCtrl.inMenu
        contentWidth: width
        contentHeight: gameColumn.implicitHeight + 24
        clip: true

        Column {
            id: gameColumn
            width: parent.width
            spacing: 12
            topPadding: 8
            bottomPadding: 16

            // Top bar com estatísticas
            Row {
                spacing: 8
                anchors.horizontalCenter: parent.horizontalCenter
                Rectangle {
                    width: 100; height: 38; color: "#16a085"; radius: 8
                    Text { anchors.centerIn: parent; text: "Jogadas: " + gameCtrl.moveCount; color: "white"; font.bold: true }
                }
                Rectangle {
                    width: 100; height: 38; color: "#3498db"; radius: 8
                    Text { anchors.centerIn: parent; text: "Pontos: " + gameCtrl.score; color: "white"; font.bold: true }
                }
                Rectangle {
                    width: 100; height: 38; color: "#34495e"; radius: 8; border.color: "#bdc3c7"
                    Text { anchors.centerIn: parent; text: gameCtrl.dangerLevel; color: "white"; font.bold: true; font.pixelSize: 12 }
                }
            }

            // Notificação
            Rectangle {
                id: notificationPopup
                width: root.boardSize; height: 35
                color: "#e67e22"; radius: 5; opacity: 0.0
                anchors.horizontalCenter: parent.horizontalCenter
                Behavior on opacity { NumberAnimation { duration: 250 } }
                Text { id: notificationText; anchors.centerIn: parent; color: "white"; font.bold: true }
            }

            // Fila de passageiros
            Column {
                spacing: 6
                anchors.horizontalCenter: parent.horizontalCenter
                Text { text: "Próximos Passageiros:"; color: "white"; font.bold: true; anchors.horizontalCenter: parent.horizontalCenter }
                Flickable {
                    width: root.boardSize; height: 45
                    contentWidth: passengerRow.implicitWidth; contentHeight: height; clip: true
                    flickableDirection: Flickable.HorizontalFlick
                    Row {
                        id: passengerRow; spacing: 10
                        Repeater {
                            model: gameCtrl.passengerQueue
                            Rectangle {
                                width: 35; height: 35; radius: 17.5
                                color: modelData.color
                                border.color: index === 0 ? "#f1c40f" : "white"
                                border.width: index === 0 ? 3 : 1
                                Text { anchors.centerIn: parent; text: index === 0 ? "🏃" : (index+1); font.pixelSize: index===0?16:11; color:"white" }
                            }
                        }
                    }
                }
            }

            // Plataformas
            Item {
                width: root.boardSize; height: 80
                anchors.horizontalCenter: parent.horizontalCenter
                Row {
                    id: slotsRow; spacing: 6; anchors.fill: parent
                    Repeater {
                        model: gameCtrl.numSlots
                        Rectangle {
                            width: (root.boardSize - slotsRow.spacing * (gameCtrl.numSlots-1)) / gameCtrl.numSlots
                            height: parent.height-10; color: "#34495e"; border.color: "#95a5a6"; border.width: 2; radius: 8
                            Text { anchors.bottom: parent.bottom; anchors.bottomMargin: 4; anchors.horizontalCenter: parent.horizontalCenter; text: "S-"+(index+1); color:"#95a5a6"; font.pixelSize:10 }
                        }
                    }
                }
                Repeater {
                    model: gameCtrl.parkedBuses
                    Rectangle {
                        property real slotW: (root.boardSize - 6 * (gameCtrl.numSlots-1)) / gameCtrl.numSlots
                        width: slotW; height: 70; radius: 6; border.color: "#1a1a1a"; border.width: 2
                        color: modelData.color
                        x: modelData.slotIndex * (slotW + 6); y: 0
                        Text { anchors.centerIn: parent; text: modelData.currentPassengers + "/" + modelData.capacity; color:"white"; font.bold: true; font.pixelSize:11 }
                    }
                }
            }

            // Tabuleiro
            Rectangle {
                width: root.boardSize; height: root.boardSize
                color: "#34495e"; border.color: "#ecf0f1"; border.width: 2; clip: true; radius: 8
                anchors.horizontalCenter: parent.horizontalCenter

                Grid {
                    anchors.fill: parent; rows: gameCtrl.rows; columns: gameCtrl.cols
                    Repeater {
                        model: gameCtrl.rows * gameCtrl.cols
                        Rectangle {
                            width: root.boardSize / gameCtrl.cols
                            height: root.boardSize / gameCtrl.rows
                            color: "transparent"; border.color: "#2c3e50"; border.width: 1
                        }
                    }
                }

                Repeater {
                    model: gameCtrl.buses
                    Rectangle {
                        property real cellSize: root.boardSize / gameCtrl.cols
                        property bool isHoriz: modelData.direction === "l" || modelData.direction === "r"
                        property int busLen: modelData.capacity / 2
                        visible: modelData.row !== -10
                        x: modelData.col * cellSize + 2; y: modelData.row * cellSize + 2
                        width: (isHoriz ? busLen * cellSize : cellSize) - 4
                        height: (!isHoriz ? busLen * cellSize : cellSize) - 4
                        color: modelData.color; radius: 8; border.color: "#1a1a1a"; border.width: 2
                        Behavior on x { NumberAnimation { duration: 200; easing.type: Easing.OutQuad } }
                        Behavior on y { NumberAnimation { duration: 200; easing.type: Easing.OutQuad } }
                        Text { anchors.centerIn: parent; text: "0/" + modelData.capacity; color:"white"; font.bold: true; font.pixelSize:12 }
                        MouseArea {
                            anchors.fill: parent
                            property int startX: 0, startY: 0, threshold: 15
                            onPressed: (m) => { startX = m.x; startY = m.y }
                            onReleased: (m) => {
                                let dx = m.x - startX, dy = m.y - startY
                                if (Math.abs(dx) > threshold || Math.abs(dy) > threshold) {
                                    if (modelData.direction === "r" && dx >  threshold) gameCtrl.handleBusClick(index)
                                    else if (modelData.direction === "l" && dx < -threshold) gameCtrl.handleBusClick(index)
                                    else if (modelData.direction === "d" && dy >  threshold) gameCtrl.handleBusClick(index)
                                    else if (modelData.direction === "u" && dy < -threshold) gameCtrl.handleBusClick(index)
                                } else {
                                    gameCtrl.handleBusClick(index)
                                }
                            }
                        }
                    }
                }

                // Overlay vitória/derrota
                Rectangle {
                    anchors.fill: parent
                    color: gameCtrl.gameState === "WON" ? "#8027ae60" : "#80c0392b"
                    visible: gameCtrl.gameState !== "PLAYING"
                    Column {
                        anchors.centerIn: parent; spacing: 20
                        Text { text: gameCtrl.gameState === "WON" ? "🏆 VITÓRIA!" : "💀 GAME OVER"; color:"white"; font.pixelSize:32; font.bold: true; anchors.horizontalCenter: parent.horizontalCenter }
                        Text { text: gameCtrl.gameState === "WON" ? "Nível concluído!" : "Plataformas cheias!"; color:"white"; font.pixelSize:16; anchors.horizontalCenter: parent.horizontalCenter }
                        Button {
                            text: gameCtrl.gameState === "WON" ? "Menu" : "Reiniciar"
                            anchors.horizontalCenter: parent.horizontalCenter
                            onClicked: {
                                if (gameCtrl.gameState === "WON") gameCtrl.goToMenu()
                                else gameCtrl.loadLevel(gameCtrl.currentLevel)
                            }
                        }
                    }
                }
            }
            Text { text: "Nível " + gameCtrl.currentLevel; color: "#bdc3c7"; font.pixelSize: 14; anchors.horizontalCenter: parent.horizontalCenter }
        }
    }
}