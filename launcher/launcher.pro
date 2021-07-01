# Copyright (C) 2016, 2017 Mentor Graphics Development (Deutschland) GmbH
# Copyright (c) 2018,2019 TOYOTA MOTOR CORPORATION
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

TEMPLATE = app
TARGET = launcher
QT = qml quick websockets gui-private
CONFIG += c++11 link_pkgconfig wayland-scanner
DESTDIR = $${OUT_PWD}/../package/root/bin
PKGCONFIG += libhomescreen wayland-client

CONFIG(release, debug|release) {
    QMAKE_POST_LINK = $(STRIP) --strip-unneeded $(TARGET)
}

SOURCES += \
    src/main.cpp \
    src/applicationmodel.cpp \
    src/appinfo.cpp \
    src/shell-desktop.cpp \
    src/homescreenhandler.cpp

HEADERS  += \
    src/applicationmodel.h \
    src/appinfo.h \
    src/shell-desktop.h \
    src/homescreenhandler.h

OTHER_FILES += \
    README.md

RESOURCES += \
    qml/images/images.qrc \
    qml/qml.qrc

AGL_SHELL_DESKTOP_PATH = $$system(pkg-config --variable=pkgdatadir agl-compositor-0.0.19-protocols)
WAYLANDCLIENTSOURCES += \
    $$AGL_SHELL_DESKTOP_PATH/agl-shell-desktop.xml
