// SPDX-License-Identifier: Apache-2.0
/*
 * Copyright (C) 2016, 2017 Mentor Graphics Development (Deutschland) GmbH
 * Copyright (c) 2018 TOYOTA MOTOR CORPORATION
 * Copyright (C) 2022 Konsulko Group
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

#include "applicationmodel.h"
#include "appinfo.h"
#include "AppLauncherClient.h"

int main(int argc, char *argv[])
{
    QString myname = QString("launcher");
    QGuiApplication app(argc, argv);

    // necessary to identify correctly by app_id
    app.setDesktopFileName(myname);

    QQmlApplicationEngine engine;

    AppLauncherClient* applauncher = new AppLauncherClient();
    QList<QMap<QString, QString>> apps;
    if (applauncher) {
        applauncher->listApplications(apps);
        engine.rootContext()->setContextProperty(QStringLiteral("applauncher"), applauncher);
        engine.rootContext()->setContextProperty(QStringLiteral("apps_len"), apps.size());
    } else {
        qFatal("Could not create AppLauncherClient");
    }

    ApplicationModel *appmodel = new ApplicationModel();
    if (appmodel) {
        appmodel->initAppList(apps);
        qmlRegisterSingletonType<ApplicationModel>("AppModel", 1, 0, "ApplicationModel",
                                                   [appmodel](QQmlEngine *, QJSEngine *) -> QObject * { return appmodel; });
    } else {
        qFatal("Could not create ApplicationModel");
    }
    engine.load(QUrl(QStringLiteral("qrc:/Launcher.qml")));

    return app.exec();
}
