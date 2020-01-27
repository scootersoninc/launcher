/*
 * Copyright (C) 2016 The Qt Company Ltd.
 * Copyright (C) 2016, 2017 Mentor Graphics Development (Deutschland) GmbH
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

#include "applicationmodel.h"
#include "appinfo.h"

#include <QtCore/QFile>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonArray>

#include "hmi-debug.h"

class ApplicationModel::Private
{
public:
    Private();

    void addApp(QString icon, QString name, QString id);
    void removeApp(QString id);

    QList<AppInfo> data;
};

namespace {
    QString get_icon_name(QJsonObject const &i)
    {
        QString icon = i["name"].toString().toLower();

        if ( !QFile::exists(QString(":/images/%1_active.svg").arg(icon)) ||
             !QFile::exists(QString(":/images/%1_inactive.svg").arg(icon)) )
        {
            icon = "blank";
        }
        return icon;
    }
}

ApplicationModel::Private::Private()
{
}

void ApplicationModel::Private::addApp(QString icon, QString name, QString id)
{
    HMI_DEBUG("addApp","name: %s icon: %s id: %s.", name.toStdString().c_str(), icon.toStdString().c_str(), id.toStdString().c_str());
    for(int i = 0; i < this->data.size(); ++i) {
        if(this->data[i].id() == id)
            return;
    }

    QString _icon = name.toLower();
    if ( !QFile::exists(QString(":/images/%1_active.svg").arg(_icon)) ||
         !QFile::exists(QString(":/images/%1_inactive.svg").arg(_icon)) )
    {
        _icon = "blank";
    }

    int pos = 0;
    for (pos = 0; pos < this->data.size(); ++pos) {
        if (QString::compare(this->data.at(pos).name(), name, Qt::CaseInsensitive) > 0)
            break;
    }
    this->data.insert(pos, AppInfo(_icon, name, id));
}

void ApplicationModel::Private::removeApp(QString id)
{
    HMI_DEBUG("removeApp","id: %s.",id.toStdString().c_str());
    for (int i = 0; i < this->data.size(); ++i) {
          if (this->data.at(i).id() == id) {
              this->data.removeAt(i);
              break;
          }
    }
}

ApplicationModel::ApplicationModel(QObject *parent)
    : QAbstractListModel(parent)
    , d(new Private())
{
}

ApplicationModel::~ApplicationModel()
{
    delete this->d;
}

int ApplicationModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return this->d->data.count();
}

QVariant ApplicationModel::data(const QModelIndex &index, int role) const
{
    QVariant ret;
    if (!index.isValid())
        return ret;

    switch (role) {
    case Qt::DecorationRole:
        ret = this->d->data[index.row()].iconPath();
        break;
    case Qt::DisplayRole:
        ret = this->d->data[index.row()].name();
        break;
    case Qt::UserRole:
        ret = this->d->data[index.row()].id();
        break;
    default:
        break;
    }

    return ret;
}

QHash<int, QByteArray> ApplicationModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::DecorationRole] = "icon";
    roles[Qt::DisplayRole] = "name";
    roles[Qt::UserRole] = "id";
    return roles;
}

QString ApplicationModel::id(int i) const
{
    return data(index(i), Qt::UserRole).toString();
}

QString ApplicationModel::appid(int i) const
{
    QString id = data(index(i), Qt::UserRole).toString();
    return id.split("@")[0];
}

QString ApplicationModel::name(int i) const
{
    return data(index(i), Qt::DisplayRole).toString();
}

void ApplicationModel::move(int from, int to)
{
    QModelIndex parent;
    if (to < 0 || to > rowCount()) return;
    if (from < to) {
        if (!beginMoveRows(parent, from, from, parent, to + 1)) {
            HMI_NOTICE("launcher","from : %d, to : %d. false.", from, to);
            return;
        }
        d->data.move(from, to);
        endMoveRows();
    } else if (from > to) {
        if (!beginMoveRows(parent, from, from, parent, to)) {
            HMI_NOTICE("launcher","from : %d, to : %d. false.", from, to);
            return;
        }
        d->data.move(from, to);
        endMoveRows();
    } else {
        HMI_NOTICE("launcher","from : %d, to : %d. false.", from, to);
    }
}

void ApplicationModel::updateApplist(QStringList info)
{
    QString icon = info.at(0);
    QString name = info.at(1);
    QString id = info.at(2);

    beginResetModel();
    if(icon == "") { // uninstall
        d->removeApp(id);
    }
    else {
        // new app
        d->addApp(icon, name, id);
    }
    endResetModel();
}

void ApplicationModel::initAppList(QString data)
{
    HMI_DEBUG("launcher","init application list.");
    beginResetModel();
    QJsonDocument japps = QJsonDocument::fromJson(data.toUtf8());
    for (auto const &app : japps.array()) {
        QJsonObject const &jso = app.toObject();
        auto const name = jso["name"].toString();
        auto const id = jso["id"].toString();
        auto const icon = get_icon_name(jso);

        d->addApp(icon, name, id);
    }
    endResetModel();
}
