/*
 * Copyright (c) 2020 Collabora Ltd.
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

#ifndef SHELLDESKTOP_H
#define SHELLDESKTOP_H

#include <QObject>
#include <QString>
#include <QScreen>
#include <QWindow>
#include <QDebug>
#include <memory>


#include "wayland-agl-shell-desktop-client-protocol.h"

static void
application_id_event(void *data, struct agl_shell_desktop *agl_shell_desktop,
		     const char *app_id);
static void
application_state_event(void *data, struct agl_shell_desktop *agl_shell_desktop,
                        const char *app_id, const char *app_data,
                        uint32_t app_state, uint32_t app_role);

static const struct agl_shell_desktop_listener agl_shell_desktop_listener = {
        application_id_event,
        application_state_event,
};

class Shell : public QObject
{
Q_OBJECT

public:
	std::shared_ptr<struct agl_shell_desktop> shell;
	Shell(std::shared_ptr<struct agl_shell_desktop> shell, QObject *parent = nullptr) :
		QObject(parent), shell(shell) 
	{
		struct agl_shell_desktop *agl_shell_desktop = shell.get();
		agl_shell_desktop_add_listener(agl_shell_desktop, 
					       &agl_shell_desktop_listener, this);
	}

public slots: // calls out of qml into CPP
	void activate_app(QWindow *win, const QString &app_id, const QString &app_data);
	void activate_app_by_screen(const QString &screen_name,
				    const QString &app_id,
				    const QString &app_data);
	void deactivate_app(const QString &app_id);
	void set_window_props(QWindow *win, const QString &app_id,
			      uint32_t props, int x, int y);
};

static void
application_id_event(void *data, struct agl_shell_desktop *agl_shell_desktop,
		     const char *app_id)
{
        Shell *aglShell = static_cast<Shell *>(data);
        (void) agl_shell_desktop;

        qInfo() << "app_id: " << app_id;

	// this ain't necessary in case the default policy API will activate
	// applications by default (when they are started) but if that is not
	// the case we can use this event handler to activate the application
	// as this event is sent when the application is created (when the
	// app surface is created that is)
	QString qstr_app_id = QString::fromUtf8(app_id, -1);
	aglShell->activate_app(nullptr, qstr_app_id, nullptr);
}

static void
application_state_event(void *data, struct agl_shell_desktop *agl_shell_desktop,
                        const char *app_id, const char *app_data,
                        uint32_t app_state, uint32_t app_role)
{
	(void) data;
	(void) agl_shell_desktop;
	(void) app_id;
	(void) app_data;
	(void) app_state;
	(void) app_role;
}

#endif // SHELLDESKTOP_H
