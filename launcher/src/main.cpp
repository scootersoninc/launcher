/*
 * Copyright (C) 2016, 2017 Mentor Graphics Development (Deutschland) GmbH
 * Copyright (c) 2018 TOYOTA MOTOR CORPORATION
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

#include <QGuiApplication>
#include <QCommandLineParser>
#include <QtCore/QUrlQuery>
#include <QtGui/QGuiApplication>
#include <QtQml/QQmlApplicationEngine>
#include <QtQml/QQmlContext>
#include <QtQml/qqml.h>
#include <QQuickWindow>
#include <QThread>

#include <qlibwindowmanager.h>
#include "applicationmodel.h"
#include "appinfo.h"
#include "homescreenhandler.h"
#include "hmi-debug.h"

int main(int argc, char *argv[])
{
    QString myname = QString("launcher");
    QGuiApplication a(argc, argv);

    QCoreApplication::setOrganizationDomain("LinuxFoundation");
    QCoreApplication::setOrganizationName("AutomotiveGradeLinux");
    QCoreApplication::setApplicationName(myname);
    QCoreApplication::setApplicationVersion("0.1.0");

    QCommandLineParser parser;
    parser.addPositionalArgument("port", a.translate("main", "port for binding"));
    parser.addPositionalArgument("secret", a.translate("main", "secret for binding"));
    parser.addHelpOption();
    parser.addVersionOption();
    parser.process(a);
    QStringList positionalArguments = parser.positionalArguments();

    int port = 1700;
    QString token = "wm";

    if (positionalArguments.length() == 2) {
        port = positionalArguments.takeFirst().toInt();
        token = positionalArguments.takeFirst();
    }

    HMI_DEBUG("launcher","port = %d, token = %s", port, token.toStdString().c_str());

    // import C++ class to QML
    qmlRegisterType<ApplicationModel>("AppModel", 1, 0, "ApplicationModel");

    QLibWindowmanager* layoutHandler = new QLibWindowmanager();
    if(layoutHandler->init(port,token) != 0){
        exit(EXIT_FAILURE);
    }

    AGLScreenInfo screenInfo(layoutHandler->get_scale_factor());

    if (layoutHandler->requestSurface(myname) != 0) {
        exit(EXIT_FAILURE);
    }

    layoutHandler->set_event_handler(QLibWindowmanager::Event_SyncDraw, [layoutHandler, myname](json_object *object) {
        layoutHandler->endDraw(myname);
    });

    HomescreenHandler* homescreenHandler = new HomescreenHandler();
    homescreenHandler->init(port, token.toStdString().c_str(), layoutHandler, myname);

    QUrl bindingAddress;
    bindingAddress.setScheme(QStringLiteral("ws"));
    bindingAddress.setHost(QStringLiteral("localhost"));
    bindingAddress.setPort(port);
    bindingAddress.setPath(QStringLiteral("/api"));

    QUrlQuery query;
    query.addQueryItem(QStringLiteral("token"), token);
    bindingAddress.setQuery(query);

    const QByteArray hack_delay = qgetenv("HMI_LAUNCHER_STARTUP_DELAY");
    int delay_time = 1;

    if (!hack_delay.isEmpty()) {
       delay_time = (QString::fromLocal8Bit(hack_delay)).toInt();
    }

    QThread::sleep(delay_time);
    qDebug("Sleep %d sec to resolve race condtion between HomeScreen and Launcher", delay_time);

    // mail.qml loading
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty(QStringLiteral("layoutHandler"), layoutHandler);
    engine.rootContext()->setContextProperty(QStringLiteral("homescreenHandler"), homescreenHandler);
    engine.rootContext()->setContextProperty(QStringLiteral("screenInfo"), &screenInfo);
    engine.load(QUrl(QStringLiteral("qrc:/Launcher.qml")));
    homescreenHandler->getRunnables();

    QObject *root = engine.rootObjects().first();
    QQuickWindow *window = qobject_cast<QQuickWindow *>(root);
    QObject::connect(window, SIGNAL(frameSwapped()), layoutHandler, SLOT(slotActivateSurface()));

    return a.exec();
}
