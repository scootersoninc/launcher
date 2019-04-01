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

#include <QFileInfo>
#include "homescreenhandler.h"
#include <functional>
#include "hmi-debug.h"

void* HomescreenHandler::myThis = 0;

HomescreenHandler::HomescreenHandler(QObject *parent) :
    QObject(parent),
    mp_hs(NULL)
{

}

HomescreenHandler::~HomescreenHandler()
{
    if (mp_hs != NULL) {
        delete mp_hs;
    }
}

void HomescreenHandler::init(int port, const char *token, QLibWindowmanager *qwm, QString myname)
{
    myThis = this;
    mp_qwm = qwm;
    m_myname = myname;

    mp_hs = new LibHomeScreen();
    mp_hs->init(port, token);
    mp_hs->registerCallback(nullptr, HomescreenHandler::onRep_static);

    mp_hs->set_event_handler(LibHomeScreen::Event_ShowWindow,[this](json_object *object){
        HMI_DEBUG("Launcher","Surface launcher got Event_ShowWindow\n");
        mp_qwm->activateWindow(m_myname);
    });

    mp_hs->set_event_handler(LibHomeScreen::Event_AppListChanged,[this](json_object *object){
        HMI_DEBUG("Launcher","laucher: appListChanged [%s]\n", json_object_to_json_string(object));

        struct json_object *obj_param, *obj_oper, *obj_data;
        if(json_object_object_get_ex(object, "parameter", &obj_param)
        && json_object_object_get_ex(obj_param, "operation", &obj_oper)
        && json_object_object_get_ex(obj_param, "data", &obj_data)) {
            QString oper = json_object_get_string(obj_oper);
            if(oper == "uninstall") {
                /* { "application_id": "launcher",
                 *   "type": "application-list-changed",
                 *   "parameter":{
                 *      "operation": "uninstall",
                 *      "data": "onstestapp@0.1"
                 *   }
                 * } */
                QString id = json_object_get_string(obj_data);
                QStringList info;
                // icon, name, id
                info << "" << "" << id;
                emit appListUpdate(info);
            }
            else if(oper == "install") {
                /* { "application_id": "launcher",
                 *   "type": "application-list-changed",
                 *   "parameter": {
                 *      "operation": "install",
                 *      "data": {
                 *          "description":"This is a demo onstestapp application",
                 *          "name": "onstestapp",
                 *          "shortname": "",
                 *          "id": "onstestapp@0.1",
                 *          "version": "0.1",
                 *          "author": "Qt",
                 *          "author-email": "",
                 *          "width": "",
                 *          "height": "",
                 *          "icon": "\/var\/local\/lib\/afm\/applications\/onstestapp\/0.1\/icon.svg",
                 *          "http-port": 31022
                 *      }
                 *    }
                 * } */
                struct json_object *obj_icon, *obj_name, *obj_id;
                if(json_object_object_get_ex(obj_data, "icon", &obj_icon)
                && json_object_object_get_ex(obj_data, "name", &obj_name)
                && json_object_object_get_ex(obj_data, "id", &obj_id)) {
                    QString icon = json_object_get_string(obj_icon);
                    QString name = json_object_get_string(obj_name);
                    QString id = json_object_get_string(obj_id);
                    QStringList info;
                    // icon, name, id
                    info << icon << name << id;
                    emit appListUpdate(info);
                }
            }
            else {
                HMI_ERROR("Launcher","error operation.\n");
            }
        }
    });
}

void HomescreenHandler::tapShortcut(QString application_id)
{
    HMI_DEBUG("Launcher","tapShortcut %s", application_id.toStdString().c_str());
    struct json_object* j_json = json_object_new_object();
    struct json_object* value;
    value = json_object_new_string("normal.full");
    json_object_object_add(j_json, "area", value);

    mp_hs->showWindow(application_id.toStdString().c_str(), j_json);
}

void HomescreenHandler::onRep_static(struct json_object* reply_contents)
{
    static_cast<HomescreenHandler*>(HomescreenHandler::myThis)->onRep(reply_contents);
}

void HomescreenHandler::onRep(struct json_object* reply_contents)
{
    HMI_DEBUG("launcher","HomeScreen onReply %s", json_object_to_json_string(reply_contents));
    struct json_object *obj_res, *obj_verb;
    if(json_object_object_get_ex(reply_contents, "response", &obj_res)
    && json_object_object_get_ex(obj_res, "verb", &obj_verb)
    && !strcasecmp("getRunnables", json_object_get_string(obj_verb))) {
        struct json_object *obj_data;
        if(json_object_object_get_ex(obj_res, "data", &obj_data)) {
            HMI_DEBUG("launcher","HomescreenHandler emit initAppList");
            QString data = json_object_to_json_string(obj_data);
            emit initAppList(data);
        }
    }
}

void HomescreenHandler::getRunnables(void)
{
    mp_hs->getRunnables();
}
