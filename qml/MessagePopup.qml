import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root
    property alias text: labelText.text
    property alias duration: timer.interval
    color: "#d2d4d2"
    border.width: 1
    border.color: "#979997"
    radius: 10
    width: 300
    height: labelText.implicitHeight + 20
    opacity: 0
    Behavior on opacity { NumberAnimation {} }
    visible: opacity > 0
    Timer {
        id: timer
        interval: 1000
        running: false
        triggeredOnStart: false
        onTriggered: {
            opacity = 0
        }
    }
    Label {
        id: labelText
        anchors.centerIn: parent
        anchors.margins: 10
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        opacity: parent.opacity
    }
    MouseArea {
        anchors.fill: parent
        Accessible.ignored: true
        onClicked: {
            root.opacity = 0
        }
    }
    function show(messageText, duration) {
        if (messageText === null) {
            return;
        }

        labelText.text = messageText;

        if (duration) {
            duration = parseInt(duration);

            if (!isNaN(duration)) {
                timer.interval = parseInt(duration);
            }
        }

        opacity = 1;
        timer.start();
    }
}
