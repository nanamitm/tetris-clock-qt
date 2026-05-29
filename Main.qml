import QtQuick
import QtQuick.Window

Window {
    id: root
    title: "Tetris Clock"
    color: "#0d0d0d"
    visible: true
    width: 1050
    height: 420

    // Android ではフルスクリーンで起動
    Component.onCompleted: {
        if (Qt.platform.os === "android") visibility = Window.FullScreen
    }

    readonly property real cellSize: Math.min(
        width  * 0.90 / controller.cols,
        height * 0.88 / controller.rows
    )
    readonly property real boardW: cellSize * controller.cols
    readonly property real boardH: cellSize * controller.rows

    // ── Background クリック操作 ───────────────────────────────────────────────
    // 左クリック     : アニメーションリプレイ
    // 左長押し(600ms): 押している間だけ日付表示（タッチ向け）
    // 右クリック     : 日付 / 時刻トグル（マウス向け）
    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        property bool longPressed: false

        Timer {
            id: holdTimer
            interval: 600
            onTriggered: {
                parent.longPressed = true
                dateOverlay.show()
            }
        }

        onPressed: function(mouse) {
            if (mouse.button === Qt.LeftButton)
                holdTimer.start()
            else if (mouse.button === Qt.RightButton)
                dateOverlay.toggle()
        }

        onReleased: function(mouse) {
            if (mouse.button !== Qt.LeftButton) return
            holdTimer.stop()
            if (longPressed) {
                longPressed = false
                dateOverlay.hide()
            } else {
                controller.replayAnimation()
            }
        }
    }

    // ── 日付オーバーレイ（アニメーションなし・即時表示） ──────────────────
    Rectangle {
        id: dateOverlay
        anchors.fill: clockArea
        visible: false
        color: "#d0000000"
        radius: 10
        z: 5

        function show()   { visible = true;  dateText.update() }
        function hide()   { visible = false }
        function toggle() { if (visible) hide(); else show() }

        Column {
            anchors.centerIn: parent
            spacing: root.cellSize * 0.25

            Text {
                id: dateText
                anchors.horizontalCenter: parent.horizontalCenter
                font.pixelSize: root.cellSize * 2.0
                font.bold: true
                color: "#ffffff"
                function update() {
                    var d = new Date()
                    text = Qt.formatDate(d, "MM/dd")
                }
            }
            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                font.pixelSize: root.cellSize * 0.7
                color: "#aaaaaa"
                text: Qt.formatDate(new Date(), "dddd")  // 曜日
            }
        }
    }

    // ── Clock area (board + AM/PM) ────────────────────────────────────────────
    Item {
        id: clockArea
        anchors.centerIn: parent
        width:  root.boardW + (ampmLabel.visible ? ampmLabel.implicitWidth + root.cellSize * 0.5 : 0)
        height: root.boardH

        Canvas {
            id: bgGrid
            width: root.boardW; height: root.boardH
            opacity: controller.brightness
            onPaint: {
                var ctx = getContext("2d"), cs = root.cellSize
                ctx.clearRect(0, 0, width, height)
                var dot = Math.max(2, cs * 0.10)
                ctx.fillStyle = controller.gridDotColor
                for (var r = 0; r < controller.rows; r++)
                    for (var c = 0; c < controller.cols; c++)
                        ctx.fillRect(c*cs+(cs-dot)/2, r*cs+(cs-dot)/2, dot, dot)
            }
            Connections { target: root;       function onCellSizeChanged()   { bgGrid.requestPaint() } }
            Connections { target: controller; function onAppearanceChanged() { bgGrid.requestPaint() } }
        }

        Canvas {
            id: canvas
            width: root.boardW; height: root.boardH
            opacity: controller.brightness
            Connections { target: controller; function onBoardChanged() { canvas.requestPaint() } }
            Connections { target: root;       function onCellSizeChanged()   { canvas.requestPaint() } }
            Connections { target: controller; function onAppearanceChanged() { canvas.requestPaint() } }

            onPaint: {
                var ctx = getContext("2d"), cs = root.cellSize
                ctx.clearRect(0, 0, width, height)
                var board = controller.board, cols = controller.cols, rows = controller.rows
                for (var r = 0; r < rows; r++)
                    for (var c = 0; c < cols; c++) {
                        var col = board[r * cols + c]
                        if (col !== "") drawLedCell(ctx, c*cs, r*cs, cs, col)
                    }
            }
            function drawLedCell(ctx, x, y, cs, color) {
                var m=Math.max(1,cs*0.07), s=cs-m*2, cx=x+m, cy=y+m, r=s*0.18, g=cs*0.18
                ctx.fillStyle = colorA(color, 0.28)
                rr(ctx, cx-g, cy-g, s+g*2, s+g*2, r+g); ctx.fill()
                ctx.fillStyle = color
                rr(ctx, cx, cy, s, s, r); ctx.fill()
                ctx.fillStyle = "rgba(255,255,255,0.22)"
                rr(ctx, cx+s*0.07, cy+s*0.07, s*0.46, s*0.28, r*0.8); ctx.fill()
            }
            function rr(ctx, x, y, w, h, r) {
                ctx.beginPath()
                ctx.moveTo(x+r,y); ctx.lineTo(x+w-r,y)
                ctx.quadraticCurveTo(x+w,y,x+w,y+r); ctx.lineTo(x+w,y+h-r)
                ctx.quadraticCurveTo(x+w,y+h,x+w-r,y+h); ctx.lineTo(x+r,y+h)
                ctx.quadraticCurveTo(x,y+h,x,y+h-r); ctx.lineTo(x,y+r)
                ctx.quadraticCurveTo(x,y,x+r,y); ctx.closePath()
            }
            function colorA(hex, a) {
                return "rgba("+parseInt(hex.substr(1,2),16)+","
                              +parseInt(hex.substr(3,2),16)+","
                              +parseInt(hex.substr(5,2),16)+","+a+")"
            }
        }

        Text {
            id: ampmLabel
            visible: !controller.use24Hour
            anchors.left: canvas.right
            anchors.leftMargin: root.cellSize * 0.5
            anchors.verticalCenter: canvas.verticalCenter
            text: new Date().getHours() < 12 ? "AM" : "PM"
            color: "#888"; font.pixelSize: root.cellSize * 0.65; font.bold: true
            Connections {
                target: controller
                function onBoardChanged() {
                    ampmLabel.text = new Date().getHours() < 12 ? "AM" : "PM"
                }
            }
        }
    }

    // ── Settings panel (top-right, expands downward) ──────────────────────────
    Rectangle {
        id: panel
        property bool open: false
        anchors { right: parent.right; top: parent.top; margins: 10 }
        width:  open ? 240 : 30
        height: open ? gearRow.height + panelContent.implicitHeight + 14 : 30
        color: "#1c1c1c"
        border { color: "#444"; width: 1 }
        radius: 6
        clip: true
        Behavior on width  { NumberAnimation { duration: 150 } }
        Behavior on height { NumberAnimation { duration: 150 } }

        // ── Gear / ✕ row (always visible at top) ──
        Item {
            id: gearRow
            width: parent.width; height: 30
            Text {
                anchors { right: parent.right; verticalCenter: parent.verticalCenter; rightMargin: 9 }
                text: panel.open ? "✕" : "⚙"
                color: gearArea.containsMouse ? "#ccc" : "#666"
                font.pixelSize: 14
            }
            MouseArea {
                id: gearArea; anchors.fill: parent; hoverEnabled: true
                onClicked: panel.open = !panel.open
            }
        }

        // ── Content (below gear row) ──
        Column {
            id: panelContent
            anchors { left: parent.left; right: parent.right; top: gearRow.bottom
                      leftMargin: 10; rightMargin: 10; topMargin: 2 }
            visible: panel.open
            spacing: 8

            PanelButton {
                width: parent.width
                label: controller.use24Hour ? "24時間 → 12時間表示" : "12時間 → 24時間表示"
                onActivated: controller.setUse24Hour(!controller.use24Hour)
            }

            PanelButton {
                width: parent.width
                visible: Qt.platform.os !== "android"
                label: root.visibility === Window.FullScreen ? "ウィンドウ表示に戻す" : "フルスクリーン"
                onActivated: root.visibility = (root.visibility === Window.FullScreen)
                             ? Window.Windowed : Window.FullScreen
            }

            PanelSlider {
                width: parent.width; label: "明るさ"
                value: controller.brightness; from: 0.2; to: 1.0
                onMoved: function(v) { controller.setBrightness(v) }
            }

            PanelSlider {
                width: parent.width; label: "速度"
                value: controller.dropSpeed; from: 0.3; to: 2.0
                onMoved: function(v) { controller.setDropSpeed(v) }
            }

            // ── カラーテーマ ──
            Column {
                width: parent.width; spacing: 5
                Text { text: "カラーテーマ"; color: "#777"; font.pixelSize: 10 }
                Grid {
                    columns: 5; spacing: 4; width: parent.width
                    Repeater {
                        model: ["クラシック","モノクロ","パステル","ネオン","レトロ"]
                        Rectangle {
                            width: (panelContent.width - 16) / 5
                            height: 24; radius: 4
                            color:  controller.colorTheme === index ? "#444" : "#252525"
                            border { color: controller.colorTheme === index ? "#ccc" : "#444"; width: 1 }
                            Text {
                                anchors.centerIn: parent
                                text: modelData; color: "#aaa"; font.pixelSize: 9
                            }
                            MouseArea {
                                anchors.fill: parent
                                onClicked: controller.setColorTheme(index)
                            }
                        }
                    }
                }
            }

            // ── グリッドドット色 ──
            Column {
                width: parent.width; spacing: 5
                Text { text: "グリッドドット色"; color: "#777"; font.pixelSize: 10 }
                Row {
                    spacing: 6
                    property var dotColors: ["#252525","#1a1a2e","#1a2e1a","#2e1a1a","#2e2a1a","#1a2a2a"]
                    Repeater {
                        model: parent.dotColors
                        Rectangle {
                            width: 22; height: 22; radius: 11
                            color: modelData
                            border {
                                color: controller.gridDotColor === modelData ? "#fff" : "#555"
                                width: controller.gridDotColor === modelData ? 2 : 1
                            }
                            MouseArea {
                                anchors.fill: parent
                                onClicked: controller.setGridDotColor(modelData)
                            }
                        }
                    }
                }
            }
        }
    }

    // ── Components ────────────────────────────────────────────────────────────
    component PanelButton: Rectangle {
        id: btnRoot
        property string label: ""
        signal activated()
        height: 26; radius: 4
        color: btnArea.containsMouse ? "#333" : "#252525"
        border { color: "#555"; width: 1 }
        Text { anchors.centerIn: parent; text: btnRoot.label; color: "#aaa"; font.pixelSize: 11 }
        MouseArea { id: btnArea; anchors.fill: parent; hoverEnabled: true
            onClicked: btnRoot.activated() }
    }

    component PanelSlider: Column {
        id: sliderRoot
        property string label: ""
        property real   value: 0.5
        property real   from:  0.0
        property real   to:    1.0
        signal moved(real v)
        spacing: 4

        Text { text: sliderRoot.label; color: "#777"; font.pixelSize: 10 }

        Item {
            id: track
            width: sliderRoot.width; height: 20

            Rectangle {   // background
                anchors.verticalCenter: parent.verticalCenter
                width: parent.width; height: 3; radius: 1; color: "#333"
            }
            Rectangle {   // filled portion
                anchors.verticalCenter: parent.verticalCenter
                width: Math.max(0, knob.x + knob.width / 2)
                height: 3; radius: 1; color: "#666"
            }
            Rectangle {   // knob
                id: knob
                width: 14; height: 14; radius: 7
                anchors.verticalCenter: parent.verticalCenter
                color: dragArea.pressed ? "#ccc" : (dragArea.containsMouse ? "#999" : "#777")
                x: (sliderRoot.value - sliderRoot.from)
                   / (sliderRoot.to - sliderRoot.from)
                   * (track.width - width)
            }
            MouseArea {
                id: dragArea
                anchors.fill: parent
                hoverEnabled: true
                function emit(mx) {
                    var ratio = Math.max(0, Math.min(1, mx / track.width))
                    sliderRoot.moved(sliderRoot.from + ratio * (sliderRoot.to - sliderRoot.from))
                }
                onPressed:           emit(mouseX)
                onPositionChanged:   if (pressed) emit(mouseX)
            }
        }
    }
}
