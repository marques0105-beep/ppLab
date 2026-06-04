import QtQuick
import QtQuick.Controls

Window {
    width: 600
    height: 960
    visible: true
    title: "Park Out - Projeto Final"
    color: "#2c3e50"

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

    
    // ECRÃ DE SELEÇÃO DE NÍVEIS (MENU INICIAL)
    Column {
        id: menuScreen
        anchors.centerIn: parent
        spacing: 30
        visible: gameCtrl.inMenu

        Text {
            text: "PARK OUT"
            font.pixelSize: 50
            color: "#1abc9c"
            font.bold: true
            anchors.horizontalCenter: parent
        }

        Text {
            text: "Escolha um Nível para Jogar:"
            font.pixelSize: 18
            color: "#ecf0f1"
            anchors.horizontalCenter: parent
        }

        Repeater {
            model: [1, 2, 3]

            Rectangle {
                width: 250; height: 80
                color: "#34495e"
                border.color: "#1abc9c"
                border.width: 2
                radius: 10
                anchors.horizontalCenter: parent

                Column {
                    anchors.centerIn: parent
                    spacing: 5

                    Text {
                        text: "Nível " + modelData
                        color: "white"
                        font.bold: true
                        font.pixelSize: 18
                        anchors.horizontalCenter: parent
                    }

                    Text {
                        text: "Recorde: " + gameCtrl.getLevelHighScore(modelData) + " pts"
                        color: "#bdc3c7"
                        font.pixelSize: 12
                        anchors.horizontalCenter: parent
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: gameCtrl.loadLevelAsync(modelData)
                }
            }
        }
    }

 
    // ECRÃ PRINCIPAL DO JOGO (TABULEIRO)
    Column {
        id: gameScreen
        anchors.centerIn: parent
        spacing: 20
        visible: !gameCtrl.inMenu

        Row {
            spacing: 12
            anchors.horizontalCenter: parent

            Button {
                text: "⬅ Menu"
                onClicked: gameCtrl.goToMenu()
            }

            // [1] BOTÃO RESTART EM TEMPO REAL ADICIONADO AQUI
            Button {
                text: "🔄 Reiniciar"
                onClicked: gameCtrl.setupTestLevel()
            }

            Rectangle {
                width: 100; height: 40
                color: "#16a085"
                radius: 8
                Text { anchors.centerIn: parent; text: "Jogadas: " + gameCtrl.moveCount; color: "white"; font.bold: true }
            }

            Rectangle {
                width: 110; height: 40
                color: "#3498db"
                radius: 8
                Text { anchors.centerIn: parent; text: "Pontos: " + gameCtrl.score; color: "white"; font.bold: true }
            }

            Rectangle {
                width: 110; height: 40
                color: "#34495e"; radius: 8; border.color: "#bdc3c7"
                Text { anchors.centerIn: parent; text: gameCtrl.dangerLevel; color: "white"; font.bold: true }
            }
        }

        Rectangle {
            id: notificationPopup
            width: 390; height: 35
            color: "#e67e22"; radius: 5; opacity: 0.0; anchors.horizontalCenter: parent
            Behavior on opacity { NumberAnimation { duration: 250 } }
            Text { id: notificationText; anchors.centerIn: parent; text: ""; color: "white"; font.bold: true }
        }

        Column {
            spacing: 5
            anchors.horizontalCenter: parent
            Text { text: "Próximos Passageiros:"; color: "#ecf0f1"; font.pixelSize: 14; font.bold: true }
            Row {
                spacing: 15
                Repeater {
                    model: gameCtrl.passengerQueue
                    Rectangle {
                        width: 35; height: 35; radius: 17.5; color: modelData
                        border.color: index === 0 ? "#f1c40f" : "white"; border.width: index === 0 ? 3 : 1
                        Text { anchors.centerIn: parent; text: index === 0 ? "🏃" : index + 1; font.pixelSize: index === 0 ? 16 : 12; color: "white" }
                    }
                }
            }
        }

        Item {
            width: 390; height: 90
            anchors.horizontalCenter: parent
            Row {
                id: slotsRow; spacing: 12; anchors.fill: parent
                Repeater {
                    model: 6
                    Rectangle {
                        width: 55; height: 90; color: "transparent"; border.color: "#95a5a6"; border.width: 2; radius: 8
                        Text { anchors.bottom: parent.bottom; anchors.bottomMargin: 5; anchors.horizontalCenter: parent.horizontalCenter; text: "Slot " + (index + 1); color: "#95a5a6"; font.pixelSize: 10 }
                    }
                }
            }
            Repeater {
                model: gameCtrl.parkedBuses
                Rectangle {
                    width: 55; height: 70; radius: 5; border.color: "white"; border.width: 2; color: modelData.color
                    x: modelData.slotIndex * (55 + 12); y: 10
                    Text { anchors.centerIn: parent; text: modelData.currentPassengers + " / " + modelData.capacity; color: "white"; font.bold: true; font.pixelSize: 11 }
                }
            }
        }

        Rectangle {
            width: 400; height: 400; color: "#34495e"; border.color: "#ecf0f1"; border.width: 2; clip: true; anchors.horizontalCenter: parent

            Grid {
                anchors.fill: parent; rows: gameCtrl.rows; columns: gameCtrl.cols
                Repeater {
                    model: gameCtrl.rows * gameCtrl.cols
                    Rectangle { width: 400 / gameCtrl.cols; height: 400 / gameCtrl.rows; color: "transparent"; border.color: "#465d6b"; border.width: 1 }
                }
            }

            Rectangle { width: parent.width; height: 4; color: "#27ae60"; anchors.top: parent.top }
            Rectangle { width: 4; height: parent.height; color: "#27ae60"; anchors.right: parent.right }

            Repeater {
                model: gameCtrl.buses
                Rectangle {
                    property double cellSize: 400 / gameCtrl.cols
                    x: modelData.col * cellSize; y: modelData.row * cellSize
                    width: modelData.direction === "h" ? (modelData.capacity / 2) * cellSize : cellSize
                    height: modelData.direction === "v" ? (modelData.capacity / 2) * cellSize : cellSize
                    color: modelData.color; radius: 5; border.color: "white"; border.width: 2

                    Behavior on x { NumberAnimation { duration: 200; easing.type: Easing.OutQuad } }
                    Behavior on y { NumberAnimation { duration: 200; easing.type: Easing.OutQuad } }

                    Text { anchors.centerIn: parent; text: modelData.capacity; color: "white"; font.bold: true }

                    // [2] DETETOR DE CLIQUE E ARRASTO MULTI-PLATAFORMA ATUALIZADO
                    MouseArea {
                        anchors.fill: parent

                        property int startX: 0
                        property int startY: 0
                        property int threshold: 15  // Sensibilidade do arrasto em píxeis
                        property bool isDraggingGesture: false

                        onPressed: (mouse) => {
                            startX = mouse.x
                            startY = mouse.y
                            isDraggingGesture = false
                        }

                        onPositionChanged: (mouse) => {
                            let diffX = mouse.x - startX
                            let diffY = mouse.y - startY
                            // Se mover o rato/dedo além do limite, ativa o modo arrasto
                            if (Math.abs(diffX) > threshold || Math.abs(diffY) > threshold) {
                                isDraggingGesture = true
                            }
                        }

                        onReleased: (mouse) => {
                            let diffX = mouse.x - startX
                            let diffY = mouse.y - startY

                            if (isDraggingGesture) {
                                // Se foi um arrasto, valida se foi feito para a frente
                                if (modelData.direction === "h" && diffX > threshold) {
                                    gameCtrl.handleBusClick(index) // Move Horizontal para a Direita
                                } else if (modelData.direction === "v" && diffY < -threshold) {
                                    gameCtrl.handleBusClick(index) // Move Vertical para Cima
                                }
                            } else {
                                // Se não arrastou quase nada, foi apenas um clique simples
                                gameCtrl.handleBusClick(index)
                            }
                        }
                    }
                }
            }

            Rectangle {
                anchors.fill: parent; color: gameCtrl.gameState === "WON" ? "#e627ae60" : "#e6c0392b"; visible: gameCtrl.gameState !== "PLAYING"
                Column {
                    anchors.centerIn: parent; spacing: 20
                    Text { text: gameCtrl.gameState === "WON" ? "🏆 VITÓRIA!" : "💀 GAME OVER"; color: "white"; font.pixelSize: 32; font.bold: true; anchors.horizontalCenter: parent }
                    Text { text: gameCtrl.gameState === "WON" ? "Nível concluído com sucesso!" : "As plataformas ficaram cheias!"; color: "white"; font.pixelSize: 16; anchors.horizontalCenter: parent }
                    Button {
                        text: gameCtrl.gameState === "WON" ? "Voltar ao Menu" : "Tentar Novamente"
                        padding: 10; anchors.horizontalCenter: parent
                        onClicked: {
                            if (gameCtrl.gameState === "WON") {
                                gameCtrl.goToMenu();
                            } else {
                                gameCtrl.setupTestLevel();
                            }
                        }
                    }
                }
            }
        }

        Text { text: "Nível Atual: " + gameCtrl.currentLevel; color: "#bdc3c7"; font.pixelSize: 14; anchors.horizontalCenter: parent }
    }
}