/**************************************************************************
**
** Copyright (C) 2012 - 2014 BlackBerry Limited. All rights reserved.
**
** Contact: BlackBerry (qt@blackberry.com)
** Contact: KDAB (info@kdab.com)
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company.  For licensing terms and
** conditions see http://www.qt.io/terms-conditions.  For further information
** use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file.  Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, The Qt Company gives you certain additional
** rights.  These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
****************************************************************************/

#include "blackberrydeployinformation.h"

#include "blackberrydeployconfiguration.h"

#include <projectexplorer/buildconfiguration.h>
#include <projectexplorer/target.h>
#include <qmakeprojectmanager/qmakeproject.h>
#include <qmakeprojectmanager/qmakenodes.h>
#include <qtsupport/baseqtversion.h>
#include <qtsupport/qtkitinformation.h>

using namespace Qnx;
using namespace Qnx::Internal;

namespace {
const char COUNT_KEY[]      = "Qnx.BlackBerry.DeployInformationCount";
const char DEPLOYINFO_KEY[] = "Qnx.BlackBerry.DeployInformation.%1";

const char ENABLED_KEY[]        = "Qnx.BlackBerry.DeployInformation.Enabled";
const char APPDESCRIPTOR_KEY[]  = "Qnx.BlackBerry.DeployInformation.AppDescriptor";
const char PACKAGE_KEY[]        = "Qnx.BlackBerry.DeployInformation.Package";
const char PROFILE_KEY[]        = "Qnx.BlackBerry.DeployInformation.ProFile";
const char TARGET_KEY[]         = "Qnx.BlackBerry.DeployInformation.Target";
const char SOURCE_KEY[]         = "Qnx.BlackBerry.DeployInformation.Source";
}

Utils::FileName BarPackageDeployInformation::appDescriptorPath() const
{
    if (userAppDescriptorPath.isEmpty())
        return Utils::FileName(sourceDir).appendPath(QLatin1String("bar-descriptor.xml"));

    return userAppDescriptorPath;
}

Utils::FileName BarPackageDeployInformation::packagePath() const
{
    if (userPackagePath.isEmpty())
        return Utils::FileName(buildDir).appendPath(targetName).appendString(QLatin1String(".bar"));

    return userPackagePath;
}

// ----------------------------------------------------------------------------

BlackBerryDeployInformation::BlackBerryDeployInformation(ProjectExplorer::Target *target)
    : QAbstractTableModel(target)
    , m_target(target)
{
    connect(project(), SIGNAL(proFilesEvaluated()), this, SLOT(updateModel()));
}

int BlackBerryDeployInformation::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_deployInformation.count();
}

int BlackBerryDeployInformation::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return ColumnCount;
}

QVariant BlackBerryDeployInformation::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= m_deployInformation.count() || index.column() >= ColumnCount)
        return QVariant();

    BarPackageDeployInformation di = m_deployInformation[index.row()];
    if (role == Qt::CheckStateRole) {
        if (index.column() == EnabledColumn)
            return di.enabled ? Qt::Checked : Qt::Unchecked;
    } else if (role == Qt::DisplayRole || role == Qt::EditRole) {
        if (index.column() == AppDescriptorColumn)
            return di.appDescriptorPath().toUserOutput();
        else if (index.column() == PackageColumn)
            return di.packagePath().toUserOutput();
    }

    return QVariant();
}

QVariant BlackBerryDeployInformation::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Vertical || role != Qt::DisplayRole)
        return QVariant();

    switch (section) {
    case EnabledColumn:
        return tr("Enabled");
    case AppDescriptorColumn:
        return tr("Application descriptor file");
    case PackageColumn:
        return tr("Package");
    default:
        return QVariant();
    }
}

bool BlackBerryDeployInformation::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;
    if (index.row() >= m_deployInformation.count() || index.column() >= ColumnCount)
        return false;

    BarPackageDeployInformation &di = m_deployInformation[index.row()];
    if (role == Qt::CheckStateRole && index.column() == EnabledColumn) {
        di.enabled = static_cast<Qt::CheckState>(value.toInt()) == Qt::Checked;
    } else if (role == Qt::EditRole) {
        if (index.column() == AppDescriptorColumn)
            di.userAppDescriptorPath = Utils::FileName::fromString(value.toString());
        else if (index.column() == PackageColumn)
            di.userPackagePath = Utils::FileName::fromString(value.toString());
    }

    emit dataChanged(index, index);
    return true;
}

Qt::ItemFlags BlackBerryDeployInformation::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QAbstractTableModel::flags(index);
    switch (index.column()) {
    case EnabledColumn:
        flags |= Qt::ItemIsEditable | Qt::ItemIsUserCheckable;
        break;
    case AppDescriptorColumn:
    case PackageColumn:
        flags |= Qt::ItemIsEditable;
        break;
    }

    return flags;
}

QList<BarPackageDeployInformation> BlackBerryDeployInformation::enabledPackages() const
{
    QList<BarPackageDeployInformation> result;

    foreach (const BarPackageDeployInformation& info, m_deployInformation) {
        if (info.enabled)
            result << info;
    }

    return result;
}

QList<BarPackageDeployInformation> BlackBerryDeployInformation::allPackages() const
{
    return m_deployInformation;
}

QVariantMap BlackBerryDeployInformation::toMap() const
{
    QVariantMap outerMap;
    outerMap[QLatin1String(COUNT_KEY)] = m_deployInformation.size();

    for (int i = 0; i < m_deployInformation.size(); ++i) {
        const BarPackageDeployInformation &deployInfo = m_deployInformation[i];

        QVariantMap deployInfoMap;
        deployInfoMap[QLatin1String(ENABLED_KEY)] = deployInfo.enabled;
        deployInfoMap[QLatin1String(APPDESCRIPTOR_KEY)] = deployInfo.userAppDescriptorPath.toString();
        deployInfoMap[QLatin1String(PACKAGE_KEY)] = deployInfo.userPackagePath.toString();
        deployInfoMap[QLatin1String(PROFILE_KEY)] = deployInfo.proFilePath.toString();
        deployInfoMap[QLatin1String(TARGET_KEY)] = deployInfo.targetName;
        deployInfoMap[QLatin1String(SOURCE_KEY)] = deployInfo.sourceDir.toString();

        outerMap[QString::fromLatin1(DEPLOYINFO_KEY).arg(i)] = deployInfoMap;
    }

    return outerMap;
}

void BlackBerryDeployInformation::fromMap(const QVariantMap &map)
{
    beginResetModel();
    m_deployInformation.clear();

    int count = map.value(QLatin1String(COUNT_KEY)).toInt();
    for (int i = 0; i < count; ++i) {
        QVariantMap innerMap = map.value(QString::fromLatin1(DEPLOYINFO_KEY).arg(i)).toMap();

        const bool enabled = innerMap.value(QLatin1String(ENABLED_KEY)).toBool();
        const QString appDescriptorPath = innerMap.value(QLatin1String(APPDESCRIPTOR_KEY)).toString();
        const QString packagePath = innerMap.value(QLatin1String(PACKAGE_KEY)).toString();
        const QString proFilePath = innerMap.value(QLatin1String(PROFILE_KEY)).toString();
        const QString targetName = innerMap.value(QLatin1String(TARGET_KEY)).toString();
        const QString sourceDir = innerMap.value(QLatin1String(SOURCE_KEY)).toString();

        BarPackageDeployInformation deployInformation(enabled, Utils::FileName::fromString(proFilePath),
                                                      Utils::FileName::fromString(sourceDir),
                                                      m_target->activeBuildConfiguration()->buildDirectory(),
                                                      targetName);
        deployInformation.userAppDescriptorPath = Utils::FileName::fromString(appDescriptorPath);
        deployInformation.userPackagePath = Utils::FileName::fromString(packagePath);
        m_deployInformation << deployInformation;
    }

    endResetModel();
}

ProjectExplorer::Target *BlackBerryDeployInformation::target() const
{
    return m_target;
}

void BlackBerryDeployInformation::updateModel()
{
    if (m_deployInformation.isEmpty()) {
        initModel();
        return;
    }

    beginResetModel();
    QList<BarPackageDeployInformation> keep;
    QList<QmakeProjectManager::QmakeProFileNode *> appNodes = project()->applicationProFiles();
    foreach (QmakeProjectManager::QmakeProFileNode *node, appNodes) {
        bool nodeFound = false;
        for (int i = 0; i < m_deployInformation.size(); ++i) {
            if (m_deployInformation[i].proFilePath == node->path()
                    && (!m_deployInformation[i].userAppDescriptorPath.isEmpty()
                        || !m_deployInformation[i].userPackagePath.isEmpty())) {
                BarPackageDeployInformation deployInformation = m_deployInformation[i];
                // In case the user resets the bar package path (or if it is empty already), we need the current build dir
                deployInformation.buildDir = m_target->activeBuildConfiguration()->buildDirectory();
                keep << deployInformation;
                nodeFound = true;
                break;
            }
        }

        if (!nodeFound)
            keep << deployInformationFromNode(node);
    }
    m_deployInformation = keep;
    endResetModel();
}

QmakeProjectManager::QmakeProject *BlackBerryDeployInformation::project() const
{
    return static_cast<QmakeProjectManager::QmakeProject *>(m_target->project());
}

void BlackBerryDeployInformation::initModel()
{
    if (!m_deployInformation.isEmpty())
        return;

    QtSupport::BaseQtVersion *version = QtSupport::QtKitInformation::qtVersion(m_target->kit());
    if (!version || !version->isValid()) {
        beginResetModel();
        m_deployInformation.clear();
        endResetModel();
        return;
    }

    const QmakeProjectManager::QmakeProFileNode *const rootNode = project()->rootQmakeProjectNode();
    if (!rootNode || rootNode->parseInProgress()) // Can be null right after project creation by wizard.
        return;

    disconnect(project(), SIGNAL(proFilesEvaluated()), this, SLOT(updateModel()));

    beginResetModel();
    m_deployInformation.clear();

    QList<QmakeProjectManager::QmakeProFileNode *> appNodes = project()->applicationProFiles();
    foreach (QmakeProjectManager::QmakeProFileNode *node, appNodes)
        m_deployInformation << deployInformationFromNode(node);

    endResetModel();
    connect(project(), SIGNAL(proFilesEvaluated()), this, SLOT(updateModel()));
}

BarPackageDeployInformation BlackBerryDeployInformation::deployInformationFromNode(QmakeProjectManager::QmakeProFileNode *node) const
{
    QmakeProjectManager::TargetInformation ti = node->targetInformation();

    const Utils::FileName buildDir = m_target->activeBuildConfiguration()->buildDirectory();

    return BarPackageDeployInformation(true, node->path(), node->path(), buildDir, ti.target);
}
