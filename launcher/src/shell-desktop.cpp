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

#include <QGuiApplication>
#include <QDebug>
#include "shell-desktop.h"
#include <qpa/qplatformnativeinterface.h>
#include <stdio.h>

static struct wl_output *
getWlOutput(QScreen *screen)
{
	QPlatformNativeInterface *native = qApp->platformNativeInterface();
	void *output = native->nativeResourceForScreen("output", screen);
	return static_cast<struct ::wl_output*>(output);
}

void
Shell::activate_app(QWindow *win, const QString &app_id,
		    const QString &app_data)
{
	QScreen *screen = nullptr;
	struct wl_output *output;

	if (!win || !win->screen()) {
		screen = qApp->screens().first();
	} else {
		screen = win->screen();
	}

	if (!screen)
		return;

	output = getWlOutput(screen);
	qDebug() << "will activate app: " << app_id;
	agl_shell_desktop_activate_app(this->shell.get(),
				       app_id.toStdString().c_str(),
				       app_data.toStdString().c_str(), output);
}

void
Shell::activate_app_by_screen(const QString &screen_name, const QString &app_id,
			      const QString &app_data)
{
	QScreen *qscreen_to_put = nullptr;
	for (auto &ss: qApp->screens()) {
		if (ss->name() == screen_name) {
			qscreen_to_put = ss;
			break;
		}
	}

	/* use the primary one */
	if (!qscreen_to_put) {
		qscreen_to_put = qApp->screens().first();
	}

	struct wl_output *output = getWlOutput(qscreen_to_put);
	qDebug() << "will activate app: " << app_id << " on output " <<
		qscreen_to_put->name();
	agl_shell_desktop_activate_app(this->shell.get(),
				       app_id.toStdString().c_str(),
				       app_data.toStdString().c_str(), output);
}

void
Shell::deactivate_app(const QString &app_id)
{
	agl_shell_desktop_deactivate_app(this->shell.get(), 
					 app_id.toStdString().c_str());
}

void
Shell::set_window_props(QWindow *win, const QString &app_id,
			uint32_t props, int x, int y)
{
	QScreen *screen = nullptr;
	struct wl_output *output;

	if (!win || !win->screen()) {
		screen = qApp->screens().first();
	} else {
		screen = win->screen();
	}

	if (!screen) {
		return;
	}

	output = getWlOutput(screen);
	agl_shell_desktop_set_app_property(this->shell.get(),
					   app_id.toStdString().c_str(),
					   props, x, y, output);
}
