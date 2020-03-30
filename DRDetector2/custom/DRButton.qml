import QtQuick 2.0
import QtQuick.Controls 2.4

Button{
    background: Rectangle {
              implicitWidth: 100
              implicitHeight: 40
              opacity: enabled ? 1 : 0.3
              border.color: down ? "#17a81a" : "#21be2b"
              border.width: 1
              radius: 2
    }
}
