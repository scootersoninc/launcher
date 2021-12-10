/*
 * Copyright (c) 2017 TOYOTA MOTOR CORPORATION
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

#include <QDBusMessage>
#include <QDBusConnection>
#include "homescreenhandler.h"
#include "hmi-debug.h"

#include <json.h>

#define APPLAUNCH_DBUS_IFACE     "org.automotivelinux.AppLaunch"
#define APPLAUNCH_DBUS_OBJECT    "/org/automotivelinux/AppLaunch"

HomescreenHandler::HomescreenHandler(QObject *parent) : QObject(parent)
{
    applaunch_iface = new org::automotivelinux::AppLaunch(APPLAUNCH_DBUS_IFACE, APPLAUNCH_DBUS_OBJECT, QDBusConnection::sessionBus(), this);
}

HomescreenHandler::~HomescreenHandler()
{
}

void HomescreenHandler::tapShortcut(QString application_id)
{
    HMI_DEBUG("Launcher","tapShortcut %s", application_id.toStdString().c_str());

    QDBusPendingReply<> reply = applaunch_iface->start(application_id);
    reply.waitForFinished();
    if (reply.isError()) {
        HMI_ERROR("Launcher","Unable to start application '%s': %s",
                  application_id.toStdString().c_str(),
                  reply.error().message().toStdString().c_str());
    }
}

void HomescreenHandler::getRunnables(void)
{
	struct json_object *json_applist;
	QString applist;
    QStringList apps;

    QDBusPendingReply<QVariantList> reply = applaunch_iface->listApplications(true);
    reply.waitForFinished();
    if (reply.isError()) {
        HMI_ERROR("Launcher","Unable to retrieve application list: %s",
                  reply.error().message().toStdString().c_str());
        return;
    } else {
        QVariantList applist_variant = reply.value();
        for (auto &v: applist_variant) {
            QString app_id;
            QString name;
            QString icon_path;
            const QDBusArgument &dbus_arg = v.value<QDBusArgument>();

            dbus_arg.beginStructure();
            dbus_arg >> app_id >> name >> icon_path;

            apps << QString("{ \"name\":\"%1\", \"id\":\"%2\", \"icon\":\"%3\" }")
                        .arg(name)
                        .arg(app_id)
                        .arg(icon_path);
            dbus_arg.endStructure();
        }
    }

    applist = QString("[ %1 ]").arg(apps.join(", "));
    json_applist = json_tokener_parse(applist.toLocal8Bit().data());
    if (json_applist) {
        QString data = json_object_to_json_string(json_applist);
        HMI_DEBUG("Launcher", "doing an emit initAppList()");
        emit initAppList(data);
    } else {
        HMI_DEBUG("Launcher", "app list is invalid!");
    }
}
