/*
 * Copyright (C) 2016 The Qt Company Ltd.
 * Copyright (C) 2016, 2017 Mentor Graphics Development (Deutschland) GmbH
 * Copyright (c) 2018,2019 TOYOTA MOTOR CORPORATION
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
import QtQuick 2.13
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.0
import QtQuick.Window 2.13
import QtGraphicalEffects 1.0
import AppModel 1.0

ApplicationWindow {

    id: root
    //width: container.width
    //height: container.height
    flags: Qt.FramelessWindowHint
    visible: true

    Item {
        id: container
        anchors.centerIn: parent
        width: Window.width
        height: Window.height

        Image {
            anchors.centerIn: parent
            source: './images/AGL_HMI_Blue_Background_Car-01.png'
        }

        GridView {
            id: grid
            anchors {
                topMargin: 60; bottomMargin: 60
                leftMargin: 60; rightMargin: 60
                fill: parent
            }
            contentHeight: 320
            // change this HorizontalFlick or see Flickable documentation
            // for other possible combinations
            flickableDirection: Flickable.VerticalFlick
            snapMode: GridView.SnapOneRow
            visible: true
            cellWidth: 320
            cellHeight: 320
            interactive: apps_len > 12 ? true : false

            // the follow makes it display from left to right to allow
            // horizontal scrolling to work
            //verticalLayoutDirection: Grid.TopToBottom
            //layoutDirection: Qt.LeftToRight
            //flow: Grid.TopToBottom

            // uncomment this out if you want to highlight the currently selected item
            //highlight: Rectangle { width: 80; height: 80; color: "steelblue"; opacity: 0.3 }

            model: ApplicationModel
            delegate: Item {
                width: grid.cellWidth
                height: grid.cellHeight

                Text {
                    color: "white"
                    anchors.top: myIcon.bottom
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.margins: 20
                    font.pixelSize: 25
                    font.letterSpacing: 5
                    wrapMode: Text.WordWrap
                    horizontalAlignment: Text.AlignHCenter
                    text: qsTr(model.name.toUpperCase())
                }

                Image {
                    id: myIcon
                    anchors.top: parent.top
                    anchors.topMargin: 20
                    anchors.horizontalCenter: parent.horizontalCenter
                    // make the image/icons smaller than the grid cell size as
                    // the text below/above current cell not be on top of the
                    // current icon
                    width: 220
                    height: 220
                    source: model.icon
                    antialiasing: true
                    property string initial: model.name.substring(0,1).toUpperCase()

                    Item {
                        id: title
                        width: 125
                        height: 125
                        anchors.centerIn: parent
                        Label {
                            style: Text.Outline
                            styleColor: '#00ADDC'
                            color: 'transparent'
                            font.pixelSize: 125
                            anchors.centerIn: parent
                            anchors.horizontalCenterOffset: model.index / 3 - 1
                            anchors.verticalCenterOffset: model.index % 3 - 1
                            text: model.icon === 'blank' ? myIcon.initial : ''
                        }

                        layer.enabled: true
                        layer.effect: LinearGradient {
                            gradient: Gradient {
                                GradientStop { position: -0.5; color: "#6BFBFF" }
                                GradientStop { position: +1.5; color: "#00ADDC" }
                            }
                        }
                    }
                }

                MouseArea {
                    id: loc
                    anchors.fill: parent
                    property string currentApp: ''
                    onClicked: {
                        parent.GridView.view.currentIndex = index
                        currentApp = ApplicationModel.appid(parent.GridView.view.currentIndex)
                        console.debug("Launcher: Starting app " + currentApp)
                        applauncher.startApplication(currentApp)
                        console.debug("Launcher: Started app " + currentApp)
                    }
                }
            }
        }
    }
}
