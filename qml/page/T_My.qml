import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import FluentUI
import SAST_Evento
import "../window"

FluScrollablePage {
    launchMode: FluPageType.SingleTask

    onErrorClicked: {
        loadMyPageInfo()
    }
    errorButtonText: lang.lang_reload
    loadingText: lang.lang_loading

    function loadMyPageInfo() {
        statusMode = FluStatusViewType.Loading
        MyPageController.loadMyPageInfo()
    }

    Component.onCompleted: {
        loadMyPageInfo()
    }

    Connections {
        target: MyPageController

        function onLoadMyPageSuccessEvent() {
            statusMode = FluStatusViewType.Success
        }
    }

    Connections {
        target: MyPageController

        function onLoadMyPageErrorEvent(message) {
            errorText = message
            statusMode = FluStatusViewType.Error
        }
    }

    ColumnLayout {
        Layout.fillWidth: true

        FluArea {
            Layout.fillWidth: true
            Layout.topMargin: 20
            height: 70
            paddings: 10
            border.width: 0
            color: "transparent"
            FluClip {
                id: img
                width: 70
                height: 70
                radius: [35, 35, 35, 35]
                Image {
                    id: img_avatar
                    asynchronous: true
                    anchors.fill: parent
                    sourceSize: Qt.size(width, height)
                    source: UserHelper.avatar
                    cache: true
                }
            }
            Column {
                anchors.left: img.right
                anchors.leftMargin: 15
                spacing: 10
                FluText {
                    text: lang.lang_welcome
                    font: FluTextStyle.Subtitle
                }
                Row{
                    FluText {
                        id: text_name
                        font.pixelSize: 24
                        text: UserHelper.nickname
                    }
                    FluText {
                        anchors.bottom: parent.bottom
                        id: text_bio
                        font.pixelSize: 20
                        color: FluColors.Grey110
                        text: " | " + UserHelper.biography
                    }
                }
            }
        }

        FluArea {
            Layout.fillWidth: true
            Layout.topMargin: 10
            height: 110
            paddings: 20
            border.width: 0
            color: "transparent"

            Column {
                id: button_profile
                anchors.leftMargin: 50
                FluIconButton {
                    width: 50
                    height: 50
                    scale: 1.8
                    iconSource: FluentIcons.PersonalFolder
                    onClicked: {
                        Qt.openUrlExternally("https://link.sast.fun/home")
                    }
                }
                FluText {
                    text: lang.lang_profile
                    width: 50
                    horizontalAlignment: Text.AlignHCenter
                }
            }

            Column {
                id: button_souvenirCard
                anchors {
                    left: button_profile.right
                    leftMargin: parent.width / 3
                }

                FluIconButton {
                    width: 50
                    height: 50
                    scale: 1.8
                    iconSource: FluentIcons.Smartcard
                    onClicked: {
                        MainWindow.window.pushPage(
                                    "qrc:/qml/page/T_SouvenirCard.qml")
                    }
                }
                FluText {
                    text: lang.lang_souvenir_card
                    width: 50
                    horizontalAlignment: Text.AlignHCenter
                }
            }

            Column {
                id: button_subscribe
                anchors {
                    left: button_souvenirCard.right
                    leftMargin: parent.width / 3
                }
                FluIconButton {
                    width: 50
                    height: 50
                    scale: 1.8
                    iconSource: FluentIcons.More
                }
                FluText {
                    text: lang.lang_stay_tuned
                    width: 50
                    horizontalAlignment: Text.AlignHCenter
                }
            }
        }

        FluText {
            text: lang.lang_history
            font: FluTextStyle.Subtitle
            Layout.leftMargin: 20
        }

        GridView {
            topMargin: 10
            Layout.fillWidth: true
            implicitHeight: contentHeight
            cellHeight: 160
            cellWidth: 292
            interactive: false
            delegate: com_item
            model: EventoBriefModel
        }
    }

    Component {
        id: com_item
        Item {
            Layout.topMargin: 10
            width: 280
            height: 140
            FluArea {
                radius: 8
                width: 280
                height: 140
                anchors.centerIn: parent
                Rectangle {
                    anchors.fill: parent
                    radius: 8
                    color: {
                        if (FluTheme.dark) {
                            if (item_mouse.containsMouse) {
                                return Qt.rgba(1, 1, 1, 0.03)
                            }
                            return Qt.rgba(0, 0, 0, 0)
                        } else {
                            if (item_mouse.containsMouse) {
                                return Qt.rgba(0, 0, 0, 0.03)
                            }
                            return Qt.rgba(0, 0, 0, 0)
                        }
                    }
                }
                FluRectangle {
                    id: item_icon
                    height: 115
                    width: 115
                    radius: [6, 6, 6, 6]
                    anchors {
                        left: parent.left
                        leftMargin: 10
                        verticalCenter: parent.verticalCenter
                    }
                    FluImage {
                        anchors.fill: parent
                        source: model.url
                        fillMode: Image.PreserveAspectCrop
                    }
                }

                FluText {
                    id: item_title
                    text: model.title
                    font: FluTextStyle.Subtitle
                    elide: Text.ElideRight
                    maximumLineCount: 1
                    anchors {
                        left: item_icon.right
                        leftMargin: 12
                        top: item_icon.top
                        right: parent.right
                        rightMargin: 5
                    }
                }

                FluText {
                    id: item_time
                    anchors {
                        left: item_title.left
                        top: item_title.bottom
                        right: parent.right
                        rightMargin: 20
                        topMargin: 10
                    }
                    text: model.time
                    color: FluColors.Grey120
                    width: 110
                    wrapMode: Text.WordWrap
                    font: FluTextStyle.Caption
                    maximumLineCount: 2
                }

                FluText {
                    id: item_department
                    anchors {
                        left: item_title.left
                        right: parent.right
                        rightMargin: 20
                        top: item_time.bottom
                        topMargin: 5
                    }
                    text: model.department
                    color: FluColors.Grey120
                    elide: Text.ElideRight
                    font: FluTextStyle.Caption
                    maximumLineCount: 1
                }

                FluText {
                    id: item_desc
                    anchors {
                        left: item_title.left
                        right: parent.right
                        rightMargin: 20
                        top: item_department.bottom
                        topMargin: 8
                    }
                    text: model.description
                    color: FluColors.Grey120
                    wrapMode: Text.WordWrap
                    elide: Text.ElideRight
                    font: FluTextStyle.Caption
                    maximumLineCount: 2
                }

                MouseArea {
                    id: item_mouse
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        EventoHelper.id = model.id
                        MainWindow.window.pushPage(
                                    "qrc:/qml/page/T_EventInfo.qml")
                    }
                }
            }
        }
    }
}
