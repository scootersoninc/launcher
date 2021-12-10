/*
 * Copyright (C) 2016, 2017 Mentor Graphics Development (Deutschland) GmbH
 * Copyright (C) 2016 The Qt Company Ltd.
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

#ifndef APPINFO_H
#define APPINFO_H

#include <QObject>
#include <QtCore/QSharedDataPointer>

class AppInfo
{
    Q_GADGET
    Q_PROPERTY(QString id READ id)
    Q_PROPERTY(QString name READ name)
    Q_PROPERTY(QString iconPath READ iconPath)
public:
    AppInfo();
    AppInfo(const QString &icon, const QString &name, const QString &id);
    AppInfo(const AppInfo &other);
    virtual ~AppInfo();
    AppInfo &operator =(const AppInfo &other);
    void swap(AppInfo &other) { qSwap(d, other.d); }

    QString id() const;
    QString name() const;
    QString iconPath() const;

    void read(const QJsonObject &json);

private:
    class Private;
    QSharedDataPointer<Private> d;
};

Q_DECLARE_SHARED(AppInfo)
Q_DECLARE_METATYPE(AppInfo)
Q_DECLARE_METATYPE(QList<AppInfo>)

#endif // APPINFO_H
