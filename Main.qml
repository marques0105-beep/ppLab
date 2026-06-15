import QtQuick
import QtQuick.Controls

Window {
    id: root
    width: 480
    height: 900
    visible: true
    title: "Park Out - Projeto Final"
    color: "#2c3e50"

    property real boardSize: Math.min(width, 520) - 16

    // contador que força a reavaliação dos bindings do menu
    // sempre que o progresso é apagado ou um nível é concluído.
    property int refreshTick: 0

    Connections {
        target: gameCtrl
        function onShowNotification(message) {
            notificationText.text = message
            notificationPopup.opacity = 1.0
            notificationTimer.restart()
        }
        // sempre que o progresso muda, incrementamos o tick
        // para forçar o Repeater a chamar de novo as funções getLevelHighScore etc.
        function onProgressReset() {
            refreshTick = refreshTick + 1
        }
    }

    Timer {
        id: notificationTimer
        interval: 2000
        onTriggered: notificationPopup.opacity = 0.0
    }

    // ========== DIÁLOGO DE CONFIRMAÇÃO ==========
    Dialog {
        id: confirmResetDialog
        modal: true
        anchors.centerIn: parent
        title: "Apagar progresso?"
        standardButtons: Dialog.Yes | Dialog.No

        contentItem: Text {
            text: "Tem a certeza de que deseja apagar TODO o progresso?\n\n" +
                  "Isto inclui:\n" +
                  "• Todos os recordes de pontuação\n" +
                  "• Todos os melhores tempos\n" +
                  "• Estado de conclusão dos níveis\n\n" +
                  "Esta acção NÃO pode ser desfeita."
            color: "black"
            wrapMode: Text.WordWrap
            width: 320
        }

        onAccepted: gameCtrl.resetProgress()
    }

    // ========== MENU ==========
    Column {
        id: menuScreen
        anchors.centerIn: parent
        spacing: 24
        visible: opacity > 0
        opacity: gameCtrl.inMenu ? 1.0 : 0.0
        Behavior on opacity { NumberAnimation { duration: 300; easing.type: Easing.InOutQuad } }
        transform: Translate {
            y: gameCtrl.inMenu ? 0 : -20
            Behavior on y { NumberAnimation { duration: 300; easing.type: Easing.InOutQuad } }
        }

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
                scale: levelMouseArea.pressed ? 0.96 : 1.0
                Behavior on scale { NumberAnimation { duration: 100 } }

                // ao referenciar root.refreshTick aqui dentro,
                // qualquer alteração a essa propriedade reavalia os bindings
                // que dependem do progresso guardado.
                property int highScore: (root.refreshTick, gameCtrl.getLevelHighScore(modelData))
                property int bestTime:  (root.refreshTick, gameCtrl.getLevelBestTime(modelData))
                property bool completed: (root.refreshTick, gameCtrl.isLevelCompleted(modelData))

                Column {
                    anchors.centerIn: parent
                    spacing: 4
                    Text {
                        text: "Nível " + modelData
                        color: "white"; font.bold: true; font.pixelSize: 18
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                    Text {
                        text: "Recorde: " + highScore + " pts"
                        color: "#bdc3c7"; font.pixelSize: 12
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                    Text {
                        text: "Tempo: " + bestTime + " s"
                        color: "#bdc3c7"; font.pixelSize: 12
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                    Text {
                        text: completed ? "✅ Concluído" : "❌ Não concluído"
                        color: completed ? "#2ecc71" : "#e74c3c"
                        font.pixelSize: 12
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                }
                MouseArea {
                    id: levelMouseArea
                    anchors.fill: parent
                    onClicked: gameCtrl.loadLevelAsync(modelData)
                }
            }
        }

        // ========== APAGAR PROGRESSO ==========
        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "🗑️  Apagar Progresso"
            padding: 10
            onClicked: confirmResetDialog.open()

            contentItem: Text {
                text: parent.text
                color: "white"
                font.bold: true
                font.pixelSize: 14
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
            background: Rectangle {
                color: parent.pressed ? "#a93226" : "#c0392b"
                radius: 8
                border.color: "#7b241c"
                border.width: 2
                implicitWidth: 220
                implicitHeight: 42
            }
        }
    }

    // ========== JOGO ==========
    Flickable {
        id: gameFlickable
        anchors.fill: parent
        contentWidth: width
        contentHeight: gameColumn.implicitHeight + 24
        clip: true
        visible: opacity > 0
        opacity: gameCtrl.inMenu ? 0.0 : 1.0
        Behavior on opacity { NumberAnimation { duration: 300; easing.type: Easing.InOutQuad } }
        transform: Translate {
            y: gameCtrl.inMenu ? 20 : 0
            Behavior on y { NumberAnimation { duration: 300; easing.type: Easing.InOutQuad } }
        }

        Column {
            id: gameColumn
            width: parent.width
            spacing: 12
            topPadding: 8
            bottomPadding: 16

            Column {
                spacing: 6
                anchors.horizontalCenter: parent.horizontalCenter
                Row {
                    spacing: 10
                    anchors.horizontalCenter: parent.horizontalCenter
                    Button { text: "⬅ Menu"; onClicked: gameCtrl.goToMenu() }
                    Button { text: "🔄 Reiniciar"; onClicked: gameCtrl.setupTestLevel() }
                }
                Row {
                    spacing: 8
                    anchors.horizontalCenter: parent.horizontalCenter
                    Rectangle { width: 110; height: 38; color: "#16a085"; radius: 8
                        Text { anchors.centerIn: parent; text: "Jogadas: " + gameCtrl.moveCount; color: "white"; font.bold: true; font.pixelSize: 13 } }
                    Rectangle { width: 110; height: 38; color: "#3498db"; radius: 8
                        Text { anchors.centerIn: parent; text: "Pontos: " + gameCtrl.score; color: "white"; font.bold: true; font.pixelSize: 13 } }
                    Rectangle { width: 70; height: 38; color: "#8e44ad"; radius: 8
                        Text { anchors.centerIn: parent; color: "white"; font.bold: true; font.pixelSize: 13
                            text: { let m = Math.floor(gameCtrl.elapsedSeconds / 60); let s = gameCtrl.elapsedSeconds % 60; return (m<10?"0"+m:m)+":"+(s<10?"0"+s:s); } } }
                    Rectangle { width: 110; height: 38; color: "#34495e"; radius: 8; border.color: "#bdc3c7"
                        Text { anchors.centerIn: parent; text: gameCtrl.dangerLevel; color: "white"; font.bold: true; font.pixelSize: 12 } }
                }
            }

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
                Text { 
                    text: "Próximos Passageiros:"; 
                    color: "white"; font.pointSize: 14; font.bold: true; 
                    anchors.horizontalCenter: parent.horizontalCenter }
                Flickable {
                    width: root.boardSize;
                    height: 45
                    contentWidth: passengerRow.implicitWidth;
                    contentHeight: height;
                    clip: true;
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
                                scale: index === 0 ? (pulseAnim.running ? 1.15 : 1.0) : 1.0
                                Behavior on scale { NumberAnimation { duration: 300; easing.type: Easing.OutBack } }
                                SequentialAnimation on scale {
                                    id: pulseAnim; running: index === 0; loops: Animation.Infinite
                                    NumberAnimation { to: 1.18; duration: 600; easing.type: Easing.InOutSine }
                                    NumberAnimation { to: 1.0;  duration: 600; easing.type: Easing.InOutSine }
                                }
                                Text { anchors.centerIn: parent; text: index === 0 ? "🏃" : index + 1; font.pixelSize: index === 0 ? 16 : 11; color: "white" }
                            }
                        }
                    }
                }
            }

            // Plataformas
            Item {
                width: root.boardSize; height: 90
                anchors.horizontalCenter: parent.horizontalCenter
                Row {
                    id: slotsRow; spacing: 6; anchors.fill: parent
                    Repeater {
                        model: gameCtrl.numSlots
                        Rectangle {
                            width: (root.boardSize - (slotsRow.spacing * (gameCtrl.numSlots - 1))) / gameCtrl.numSlots
                            height: parent.height - 10
                            color: "#34495e"; border.color: "#95a5a6"; border.width: 2; radius: 8
                            Text { anchors.bottom: parent.bottom; anchors.bottomMargin: 4; anchors.horizontalCenter: parent.horizontalCenter
                                text: "S-" + (index + 1); color: "#95a5a6"; font.pixelSize: 10 }
                        }
                    }
                }
                Repeater {
                    model: gameCtrl.parkedBuses
                    Rectangle {
                        property real slotW: (root.boardSize - (slotsRow.spacing * (gameCtrl.numSlots - 1))) / gameCtrl.numSlots
                        width: slotW; height: 72; radius: 6; border.color: "#1a1a1a"; border.width: 2; color: modelData.color
                        x: modelData.slotIndex * (slotW + slotsRow.spacing); y: 0
                        Behavior on x { NumberAnimation { duration: 250; easing.type: Easing.OutQuad } }
                        Text {
                            id: passengerCountText
                            anchors.centerIn: parent
                            text: modelData.currentPassengers + "/" + modelData.capacity
                            color: "white"; font.bold: true; font.pixelSize: 11
                            Behavior on scale { NumberAnimation { duration: 150 } }
                            onTextChanged: scale = 1.2
                            onScaleChanged: if (scale == 1.2) scale = 1.0
                        }
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
                            width: root.boardSize / gameCtrl.cols; height: root.boardSize / gameCtrl.rows
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
                        property int  busLen:  modelData.capacity / 2
                        visible: modelData.row !== -10
                        x: modelData.col * cellSize + 2; y: modelData.row * cellSize + 2
                        width:  (isHoriz ? busLen * cellSize : cellSize) - 4
                        height: (!isHoriz ? busLen * cellSize : cellSize) - 4
                        color: modelData.color; radius: 8; border.color: "#1a1a1a"; border.width: 2

                        // Animações 
                        Behavior on x { NumberAnimation { duration: 400; easing.type: Easing.OutQuad } }
                        Behavior on y { NumberAnimation { duration: 400; easing.type: Easing.OutQuad } }

                        Text {
                            anchors.centerIn: parent
                            text: modelData.currentPassengers + "/" + modelData.capacity
                            color: "white"; font.bold: true; font.pixelSize: 12
                        }

                        Rectangle {
                            width: 18; height: 18; color: "white"; radius: 3; border.color: "#1a1a1a"; border.width: 1
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
                                text: modelData.direction === "l" ? "◀" : modelData.direction === "r" ? "▶" : modelData.direction === "u" ? "▲" : "▼"
                                color: "black"; font.bold: true; font.pixelSize: 10
                            }
                        }

                        MouseArea {
                            anchors.fill: parent
                            property int startX: 0; property int startY: 0; property int threshold: 15
                            onPressed: (mouse) => { startX = mouse.x; startY = mouse.y }
                            onReleased: (mouse) => {
                                let diffX = mouse.x - startX; let diffY = mouse.y - startY
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

                // Overlay vitória/derrota
                Rectangle {
                    anchors.fill: parent
                    opacity: gameCtrl.gameState !== "PLAYING" ? 1.0 : 0.0
                    scale:   gameCtrl.gameState !== "PLAYING" ? 1.0 : 0.85
                    visible: opacity > 0
                    Behavior on opacity { NumberAnimation { duration: 300; easing.type: Easing.OutQuad } }
                    Behavior on scale   { NumberAnimation { duration: 300; easing.type: Easing.OutBack } }
                    color: gameCtrl.gameState === "WON" ? "#e027ae60" : "#e0c0392b"
                    Column {
                        anchors.centerIn: parent; spacing: 20
                        Text { text: gameCtrl.gameState === "WON" ? "🏆 VITÓRIA!" : "💀 GAME OVER";
                        color: "white"; font.pixelSize: 32; font.bold: true;
                        anchors.horizontalCenter: parent.horizontalCenter }
                        Text { text: gameCtrl.gameState === "WON" ? "Nível concluído com sucesso!" : "As plataformas ficaram cheias!"; 
                        color: "white"; 
                        font.pixelSize: 16;
                        anchors.horizontalCenter: parent.horizontalCenter }
                        Button {
                            text: gameCtrl.gameState === "WON" ? "Voltar ao Menu" : "Tentar Novamente";
                            padding: 10;
                            anchors.horizontalCenter: parent.horizontalCenter
                            onClicked: { 
                                if (gameCtrl.gameState === "WON") gameCtrl.goToMenu();
                                else gameCtrl.setupTestLevel() 
                            }
                        }
                    }
                }
            }

            Text {
                text: "Nível Atual: " + gameCtrl.currentLevel; 
                color: "#bdc3c7";
                font.pixelSize: 14;
                anchors.horizontalCenter: parent.horizontalCenter }
        }
    }
}