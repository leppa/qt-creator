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

#include "blackberryrunconfigurationfactory.h"
#include "qnxconstants.h"
#include "blackberryrunconfiguration.h"
#include "blackberrydeviceconfigurationfactory.h"

#include <projectexplorer/kitinformation.h>
#include <projectexplorer/target.h>
#include <qmakeprojectmanager/qmakeproject.h>
#include <qmakeprojectmanager/qmakenodes.h>

using namespace Qnx;
using namespace Qnx::Internal;

static Utils::FileName pathFromId(Core::Id id)
{
    return Utils::FileName::fromString(id.suffixAfter(Constants::QNX_BB_RUNCONFIGURATION_PREFIX));
}

BlackBerryRunConfigurationFactory::BlackBerryRunConfigurationFactory(QObject *parent) :
    ProjectExplorer::IRunConfigurationFactory(parent)
{
}

QList<Core::Id> BlackBerryRunConfigurationFactory::availableCreationIds(ProjectExplorer::Target *parent, CreationMode mode) const
{
    using QmakeProjectManager::QmakeProject;
    if (!canHandle(parent))
        return QList<Core::Id>();

    QmakeProject *qt4Project = qobject_cast<QmakeProject *>(parent->project());
    if (!qt4Project)
        return QList<Core::Id>();

    QList<QmakeProjectManager::QmakeProFileNode *>  nodes = qt4Project->applicationProFiles();
    if (mode == AutoCreate)
        nodes = QmakeProject::nodesWithQtcRunnable(nodes);
    return QmakeProject::idsForNodes(Core::Id(Constants::QNX_BB_RUNCONFIGURATION_PREFIX),
                                     nodes);
}

QString BlackBerryRunConfigurationFactory::displayNameForId(Core::Id id) const
{
    const Utils::FileName path = pathFromId(id);
    if (path.isEmpty())
        return QString();

    if (id.name().startsWith(Constants::QNX_BB_RUNCONFIGURATION_PREFIX))
        return path.toFileInfo().completeBaseName();

    return QString();
}

bool BlackBerryRunConfigurationFactory::canCreate(ProjectExplorer::Target *parent, Core::Id id) const
{
    if (!canHandle(parent))
        return false;

    QmakeProjectManager::QmakeProject *qt4Project = qobject_cast<QmakeProjectManager::QmakeProject *>(parent->project());
    if (!qt4Project)
        return false;

    if (!id.name().startsWith(Constants::QNX_BB_RUNCONFIGURATION_PREFIX))
        return false;

    return qt4Project->hasApplicationProFile(pathFromId(id));
}

ProjectExplorer::RunConfiguration *BlackBerryRunConfigurationFactory::doCreate(ProjectExplorer::Target *parent,
                                                                      const Core::Id id)
{
    return new BlackBerryRunConfiguration(parent, id, pathFromId(id));
}

bool BlackBerryRunConfigurationFactory::canRestore(ProjectExplorer::Target *parent,
                                            const QVariantMap &map) const
{
    if (!canHandle(parent))
        return false;

    return ProjectExplorer::idFromMap(map).name().startsWith(Constants::QNX_BB_RUNCONFIGURATION_PREFIX);
}

ProjectExplorer::RunConfiguration *BlackBerryRunConfigurationFactory::doRestore(
        ProjectExplorer::Target *parent,
        const QVariantMap &map)
{
    Q_UNUSED(map);
    return new BlackBerryRunConfiguration(parent, Core::Id(Constants::QNX_BB_RUNCONFIGURATION_PREFIX),
                                          Utils::FileName());
}

bool BlackBerryRunConfigurationFactory::canClone(ProjectExplorer::Target *parent,
                                          ProjectExplorer::RunConfiguration *source) const
{
    return canCreate(parent, source->id());
}

ProjectExplorer::RunConfiguration *BlackBerryRunConfigurationFactory::clone(
        ProjectExplorer::Target *parent,
        ProjectExplorer::RunConfiguration *source)
{
    if (!canClone(parent, source))
        return 0;

    BlackBerryRunConfiguration *old = static_cast<BlackBerryRunConfiguration *>(source);
    return new BlackBerryRunConfiguration(parent, old);

}

bool BlackBerryRunConfigurationFactory::canHandle(ProjectExplorer::Target *t) const
{
    if (!t->project()->supportsKit(t->kit()))
        return false;
    if (!qobject_cast<QmakeProjectManager::QmakeProject *>(t->project()))
        return false;

    Core::Id deviceType = ProjectExplorer::DeviceTypeKitInformation::deviceTypeId(t->kit());
    if (deviceType != BlackBerryDeviceConfigurationFactory::deviceType())
        return false;

    return true;
}
