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
#include <QtGui/QGuiApplication>
#include <QtQml/QQmlApplicationEngine>
#include <QtQml/QQmlContext>
#include <QtQml/qqml.h>
#include <QQuickWindow>

#include <qlibwindowmanager.h>
#include "applicationlauncher.h"
#include "applicationmodel.h"
#include "appinfo.h"
#include "afm_user_daemon_proxy.h"
#include "qlibhomescreen.h"
#include "hmi-debug.h"

// XXX: We want this DBus connection to be shared across the different
// QML objects, is there another way to do this, a nice way, perhaps?
org::AGL::afm::user *afm_user_daemon_proxy;

namespace {

struct Cleanup {
    static inline void cleanup(org::AGL::afm::user *p) {
        delete p;
        afm_user_daemon_proxy = Q_NULLPTR;
    }
};

void noOutput(QtMsgType, const QMessageLogContext &, const QString &)
{
}

}

int main(int argc, char *argv[])
{
    QString myname = QString("launcher");
    QGuiApplication a(argc, argv);

    // use launch process
    QScopedPointer<org::AGL::afm::user, Cleanup> afm_user_daemon_proxy(new org::AGL::afm::user("org.AGL.afm.user",
                                                                                               "/org/AGL/afm/user",
                                                                                               QDBusConnection::sessionBus(),
                                                                                               0));
    ::afm_user_daemon_proxy = afm_user_daemon_proxy.data();

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

    // DBus
    qDBusRegisterMetaType<AppInfo>();
    qDBusRegisterMetaType<QList<AppInfo> >();

    QLibHomeScreen* homescreenHandler = new QLibHomeScreen();
    ApplicationLauncher *launcher = new ApplicationLauncher();
    QLibWindowmanager* layoutHandler = new QLibWindowmanager();
    if(layoutHandler->init(port,token) != 0){
        exit(EXIT_FAILURE);
    }

    if (layoutHandler->requestSurface(myname) != 0) {
        exit(EXIT_FAILURE);
    }

    layoutHandler->set_event_handler(QLibWindowmanager::Event_SyncDraw, [layoutHandler, myname](json_object *object) {
        layoutHandler->endDraw(myname);
    });

    layoutHandler->set_event_handler(QLibWindowmanager::Event_Visible, [layoutHandler, launcher](json_object *object) {
        QString label = QString(json_object_get_string(	json_object_object_get(object, "drawing_name") ));
        qDebug() << label;
        QMetaObject::invokeMethod(launcher, "setCurrent", Qt::QueuedConnection, Q_ARG(QString, label == "HomeScreen" ? "Home" : label));
    });

    layoutHandler->set_event_handler(QLibWindowmanager::Event_Invisible, [layoutHandler, launcher](json_object *object) {
        const char* label = json_object_get_string(	json_object_object_get(object, "drawing_name") );
        HMI_DEBUG("launch", "surface %s Event_Invisible", label);
    });

    homescreenHandler->init(port, token.toStdString().c_str());

    homescreenHandler->set_event_handler(QLibHomeScreen::Event_TapShortcut, [layoutHandler, myname](json_object *object){
        json_object *appnameJ = nullptr;
        if(json_object_object_get_ex(object, "application_name", &appnameJ))
        {
            const char *appname = json_object_get_string(appnameJ);
            if(myname == appname)
            {
                qDebug("Surface %s got tapShortcut\n", appname);
                layoutHandler->activateSurface(myname);
            }
        }
    });

    QUrl bindingAddress;
    bindingAddress.setScheme(QStringLiteral("ws"));
    bindingAddress.setHost(QStringLiteral("localhost"));
    bindingAddress.setPort(port);
    bindingAddress.setPath(QStringLiteral("/api"));

    QUrlQuery query;
    query.addQueryItem(QStringLiteral("token"), token);
    bindingAddress.setQuery(query);

    // mail.qml loading
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("layoutHandler", layoutHandler);
    engine.rootContext()->setContextProperty("homescreenHandler", homescreenHandler);
    engine.rootContext()->setContextProperty("launcher", launcher);
    engine.load(QUrl(QStringLiteral("qrc:/Launcher.qml")));

    QObject *root = engine.rootObjects().first();
    QQuickWindow *window = qobject_cast<QQuickWindow *>(root);
    QObject::connect(window, SIGNAL(frameSwapped()), layoutHandler, SLOT(slotActivateSurface()));

    return a.exec();
}
