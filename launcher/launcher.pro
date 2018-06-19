# Copyright (C) 2016, 2017 Mentor Graphics Development (Deutschland) GmbH
# Copyright (c) 2018 TOYOTA MOTOR CORPORATION
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
QT = qml quick dbus websockets
CONFIG += c++11 link_pkgconfig
DESTDIR = $${OUT_PWD}/../package/root/bin
PKGCONFIG += qlibwindowmanager qlibhomescreen

include(../interfaces/interfaces.pri)

SOURCES += \
    src/main.cpp \
    src/applicationmodel.cpp \
    src/appinfo.cpp \
    src/applicationlauncher.cpp

HEADERS  += \
    src/applicationlauncher.h \
    src/applicationmodel.h \
    src/appinfo.h

OTHER_FILES += \
    README.md

RESOURCES += \
    qml/images/images.qrc \
    qml/qml.qrc
