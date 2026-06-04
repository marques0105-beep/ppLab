import QtQuick
import QtQuick.Controls

Window {
    id: root
    width: 480
    height: 900
    visible: true
    title: "Park Out "
    color: "#2c3e50"

    // Board size adapts to actual window width so it fits any phone screen
    property real boardSize: Math.min(width, 520) - 16

    // Recebe notificações do C++
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

    // ==========================================
    //                   MENU
    // ==========================================
    Column {
        id: menuScreen
        anchors.centerIn: parent
        spacing: 24
        opacity: gameCtrl.inMenu ? 1 : 0
        visible: opacity > 0
        Behavior on opacity { NumberAnimation { duration: 300 } }

        Text {
            text: "PARK OUT"
            font.pixelSize: 50
            color: "#1abc9c"
            font.bold: true
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Text {
            text: "Escolha um Nível para Jogar:"
            font.pixelSize: 18
            color: "#ecf0f1"
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Repeater {
            model: [1, 2, 3]

            Rectangle {
                width: Math.min(root.width - 40, 300)
                height: 120
                color: "#34495e"
                border.color: "#1abc9c"
                border.width: 2
                radius: 10
                anchors.horizontalCenter: parent.horizontalCenter

                Column {
                    anchors.centerIn: parent
                    spacing: 4

                    Text {
                        text: "Nível " + modelData
                        color: "white"; 
                        font.bold: true; 
                        font.pixelSize: 18
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                    Text {
                        text: "Recorde: " + gameCtrl.getLevelHighScore(modelData) + " pts"
                        color: "#bdc3c7"; 
                        font.pixelSize: 12
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                    Text {
                        text: "Tempo: " + gameCtrl.getLevelBestTime(modelData) + " s"
                        color: "#bdc3c7"; 
                        font.pixelSize: 12
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                    Text {
                        text: gameCtrl.isLevelCompleted(modelData) ? "✅ Concluído" : "❌ Não concluído"
                        color: gameCtrl.isLevelCompleted(modelData) ? "#2ecc71" : "#e74c3c"
                        font.pixelSize: 12
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: gameCtrl.loadLevelAsync(modelData)
                }
            }
        }
    }

    // ==========================================
    //                  JOGO
    // =========================================
    Flickable {
        id: gameFlickable
        anchors.fill: parent
        opacity: gameCtrl.inMenu ? 0 : 1
        visible: opacity > 0
        Behavior on opacity { NumberAnimation { duration: 300 } }
        contentWidth: width
        contentHeight: gameColumn.implicitHeight + 24
        clip: true

        Column {
            id: gameColumn
            width: parent.width
            spacing: 12
            topPadding: 8
            bottomPadding: 16

            // Barra superior com botões e estatísticas
            Column {
                spacing: 6
                anchors.horizontalCenter: parent.horizontalCenter

                // Row 1: navigation buttons
                Row {
                    spacing: 10
                    anchors.horizontalCenter: parent.horizontalCenter

                    Button {
                        text: "⬅ Menu"
                        onClicked: gameCtrl.goToMenu()
                    }
                    Button {
                        text: "🔄 Reiniciar"
                        onClicked: gameCtrl.setupTestLevel()
                    }
                }

                // Row 2: stats
                Row {
                    spacing: 8
                    anchors.horizontalCenter: parent.horizontalCenter

                    Rectangle {
                        width: 110; height: 38; color: "#16a085"; radius: 8
                        Text { anchors.centerIn: parent; text: "Jogadas: " + gameCtrl.moveCount; color: "white"; font.bold: true; font.pixelSize: 13 }
                    }
                    Rectangle {
                        width: 110; height: 38; color: "#3498db"; radius: 8
                        Text { anchors.centerIn: parent; text: "Pontos: " + gameCtrl.score; color: "white"; font.bold: true; font.pixelSize: 13 }
                    }
                    Rectangle {
                        width: 70; height: 38; color: "#8e44ad"; radius: 8
                        Text {
                            anchors.centerIn: parent; color: "white"; font.bold: true; font.pixelSize: 13
                            text: {
                                var m = Math.floor(gameCtrl.elapsedSeconds / 60)
                                var s = gameCtrl.elapsedSeconds % 60
                                return (m < 10 ? "0"+m : m) + ":" + (s < 10 ? "0"+s : s)
                            }
                        }
                    }
                    Rectangle {
                        width: 110; height: 38; color: "#34495e"; radius: 8; border.color: "#bdc3c7"
                        Text { anchors.centerIn: parent; text: gameCtrl.dangerLevel; color: "white"; font.bold: true; font.pixelSize: 12 }
                    }
                }
            }

            // Notificação (popup)
            Rectangle {
                id: notificationPopup
                width: root.boardSize; height: 35
                color: "#e67e22"; radius: 5; opacity: 0.0
                anchors.horizontalCenter: parent.horizontalCenter
                Behavior on opacity { NumberAnimation { duration: 250 } }
                Text {
                    id: notificationText;
                    anchors.centerIn:parent; 
                    text: "";
                    color: "white";
                    font.bold: true
                }
            }

            // Fila de passageiros
            Column {
                spacing: 6
                anchors.horizontalCenter: parent.horizontalCenter

                Text {
                    text: "Próximos Passageiros:"
                    color: "white"; font.pointSize: 14; font.bold: true
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                Flickable {
                    width: root.boardSize
                    height: 45
                    anchors.horizontalCenter: parent.horizontalCenter
                    contentWidth: passengerRow.implicitWidth
                    contentHeight: height
                    clip: true
                    flickableDirection: Flickable.HorizontalFlick

                    Row {
                        id: passengerRow
                        spacing: 10

                        Repeater {
                            model: gameCtrl.passengerQueue
                            Rectangle {
                                width: 35; height: 35; radius: 17.5
                                color: modelData.color
                                border.color: index === 0 ? "#f1c40f" : "white"
                                border.width: index === 0 ? 3 : 1
                                scale: index === 0 ? 1.15 : 1.0
                                Behavior on scale { NumberAnimation { duration: 400; easing.type: Easing.InOutQuad } }
                                Text {
                                    anchors.centerIn: parent
                                    text: index === 0 ? "🏃" : index + 1
                                    font.pixelSize: index === 0 ? 16 : 11
                                    color: "white"
                                }
                            }
                        }
                    }
                }
            }

            // ── Platform slots ──
            Item {
                width: root.boardSize
                height: 90
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
                            color: "#34495e"; border.color: "#95a5a6"; border.width: 2; radius: 8
                            Text {
                                anchors.bottom: parent.bottom; anchors.bottomMargin: 4
                                anchors.horizontalCenter: parent.horizontalCenter
                                text: "S-" + (index + 1); color: "#95a5a6"; font.pixelSize: 10
                            }
                        }
                    }
                }

                Repeater {
                    model: gameCtrl.parkedBuses
                    Rectangle {
                        property real slotW: (root.boardSize - (6 * (gameCtrl.numSlots - 1))) / gameCtrl.numSlots
                        width: slotW; height: 72
                        radius: 6; border.color: "#1a1a1a"; border.width: 2; color: modelData.color
                        x: modelData.slotIndex * (slotW + 6); y: 0
                        Text {
                            anchors.centerIn: parent
                            text: modelData.currentPassengers + "/" + modelData.capacity
                            color: "white"; font.bold: true; font.pixelSize: 11
                        }
                    }
                }
            }

            // ── Game board ──
            Rectangle {
                width: root.boardSize; 
                height: root.boardSize
                color: "#34495e";
                border.color: "#ecf0f1"; 
                border.width: 2
                clip: true; 
                radius: 8
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

                Rectangle { anchors.fill: parent; color: "transparent"; border.color: "#27ae60"; border.width: 4 }

                Repeater {
                    model: gameCtrl.buses
                    Rectangle {
                        property real cellSize: root.boardSize / gameCtrl.cols
                        property bool isHoriz: modelData.direction === "l" || modelData.direction === "r"
                        property int  busLen: modelData.capacity / 2

                        visible: modelData.row !== -10

                        x: modelData.col * cellSize + 2
                        y: modelData.row * cellSize + 2
                        width:  (isHoriz ? busLen * cellSize : cellSize) - 4
                        height: (!isHoriz ? busLen * cellSize : cellSize) - 4
                        color: modelData.color; radius: 8
                        border.color: "#1a1a1a"; border.width: 2

                        Behavior on x { NumberAnimation { duration: 200; easing.type: Easing.OutQuad } }
                        Behavior on y { NumberAnimation { duration: 200; easing.type: Easing.OutQuad } }

                        Text {
                            anchors.centerIn: parent
                            text: "0/" + modelData.capacity
                            color: "white"; font.bold: true; font.pixelSize: 12
                        }

                        Rectangle {
                            width: 18; height: 18; color: "white"; radius: 3
                            border.color: "#1a1a1a"; border.width: 1
                            anchors.left:   modelData.direction === "l" ? parent.left   : undefined
                            anchors.right:  modelData.direction === "r" ? parent.right  : undefined
                            anchors.top:    modelData.direction === "u" ? parent.top    : undefined
                            anchors.bottom: modelData.direction === "d" ? parent.bottom : undefined
                            anchors.leftMargin:   modelData.direction === "l" ? 4 : 0
                            anchors.rightMargin:  modelData.direction === "r" ? 4 : 0
                            anchors.topMargin:    modelData.direction === "u" ? 4 : 0
                            anchors.bottomMargin: modelData.direction === "d" ? 4 : 0
                            anchors.verticalCenter:   isHoriz  ? parent.verticalCenter   : undefined
                            anchors.horizontalCenter: !isHoriz ? parent.horizontalCenter : undefined
                            Text {
                                anchors.centerIn: parent
                                text: modelData.direction === "l" ? "◀" :
                                      modelData.direction === "r" ? "▶" :
                                      modelData.direction === "u" ? "▲" : "▼"
                                color: "black"; font.bold: true; font.pixelSize: 10
                            }
                        }

                        MouseArea {
                            anchors.fill: parent
                            property int startX: 0
                            property int startY: 0
                            property int threshold: 15

                            onPressed:  (mouse) => { startX = mouse.x; startY = mouse.y }
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

                // Overlay de vitória/derrota
                Rectangle {
                    anchors.fill: parent
                    color: gameCtrl.gameState === "WON" ? "#8027ae60" : "#80c0392b"
                    opacity: gameCtrl.gameState !== "PLAYING" ? 1.0 : 0.0
                    visible: opacity > 0
                    Behavior on opacity { NumberAnimation { duration: 250 } }
                    scale: gameCtrl.gameState !== "PLAYING" ? 1.0 : 0.8
                    Behavior on scale { NumberAnimation { duration: 250; easing.type: Easing.OutBack } }

                    Column {
                        anchors.centerIn: parent; spacing: 20
                        Text {
                            text: gameCtrl.gameState === "WON" ? "🏆 VITÓRIA!" : "💀 GAME OVER"
                            color: "white"; font.pixelSize: 32; font.bold: true
                            anchors.horizontalCenter: parent.horizontalCenter
                        }
                        Text {
                            text: gameCtrl.gameState === "WON" ? "Nível concluído com sucesso!" : "As plataformas ficaram cheias!"
                            color: "white"; font.pixelSize: 16
                            anchors.horizontalCenter: parent.horizontalCenter
                        }
                        Button {
                            text: gameCtrl.gameState === "WON" ? "Voltar ao Menu" : "Tentar Novamente"
                            padding: 10
                            anchors.horizontalCenter: parent.horizontalCenter
                            onClicked: {
                                if (gameCtrl.gameState === "WON") gameCtrl.goToMenu()
                                else                               gameCtrl.setupTestLevel()
                            }
                        }
                    }
                }
            }

            Text {
                text: "Nível Atual: " + gameCtrl.currentLevel
                color: "#bdc3c7"; font.pixelSize: 14
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }
    }
}