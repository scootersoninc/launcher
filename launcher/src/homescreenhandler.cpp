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

#include "homescreenhandler.h"
#include "hmi-debug.h"

HomescreenHandler::HomescreenHandler(QObject *parent) : QObject(parent)
{
}

HomescreenHandler::~HomescreenHandler()
{
}

void HomescreenHandler::tapShortcut(QString application_id, QString output_name)
{
	HMI_DEBUG("Launcher","tapShortcut %s", application_id.toStdString().c_str());
}

void HomescreenHandler::onRep(struct json_object* reply_contents)
{
	if (reply_contents) {
		QString data = json_object_to_json_string(reply_contents);
		HMI_DEBUG("Launcher", "doing an emit initAppList()");
		emit initAppList(data);
	} else {
		HMI_DEBUG("Launcher", "reply contents is invalid!");
	}
}

void HomescreenHandler::getRunnables(void)
{
}
