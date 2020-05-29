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
import QtQuick 2.6
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.0
import QtQuick.Window 2.13
import AppModel 1.0

ApplicationWindow {
    width: container.width * container.scale
    height: container.height * container.scale

    property int pid: -1

    Item {
        id: container
        anchors.centerIn: parent
        width: Window.width
        height: Window.height
        //scale: screenInfo.scale_factor()
        scale: 1

        Image {
          anchors.centerIn: parent
          source: './images/AGL_HMI_Blue_Background_Car-01.png'
        }

    GridView {
        id: grid
        anchors {
          topMargin: 60; bottomMargin: 0 
          leftMargin: 60; rightMargin: 60
          fill: parent
        }
        contentHeight: 320
        flickableDirection: Flickable.AutoFlickDirection
        snapMode: GridView.SnapOneRow
        visible: true
        cellWidth: 320
        cellHeight: 320
        interactive: false

        model: ApplicationModel { id: applicationModel }
        delegate: IconItem {
            width: grid.cellWidth
            height: grid.cellHeight
        }

        Connections {
            target: homescreenHandler
            onAppListUpdate: {
                console.warn("applist update in Launcher.qml")
                applicationModel.updateApplist(info);
            }
        }
        Connections {
            target: homescreenHandler
            onInitAppList: {
                console.warn("applist init in Launcher.qml")
                applicationModel.initAppList(data);
            }
        }

        MouseArea {
            id: loc
            anchors.fill: parent
            property string currentId: ''
            property int newIndex: -1
            property int index: grid.indexAt(loc.mouseX, loc.mouseY)
	    property string output_screen: ''
            x: 62
            y: 264
            onPressAndHold: currentId = applicationModel.id(newIndex = index)
            onReleased: {
                if(loc.index < 0) {
                    return
                }

	        //if (applicationModel.appid(loc.index) === 'tbtnavi' ||
		//    applicationModel.appid(loc.index) === 'hvac') {
		//	output_screen = 'Virtual-1'
		//}
                if (currentId === '') {
                    homescreenHandler.tapShortcut(applicationModel.appid(loc.index), output_screen)
                } else {
                    currentId = ''
                }
            }
            onPositionChanged: {
                if (loc.currentId === '') return
                if (index < 0) return
                if (index === newIndex) return
                    applicationModel.move(newIndex, newIndex = index)
            }
        }
    }
}
}
