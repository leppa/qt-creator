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

#include "blackberryrunconfiguration.h"
#include "qnxconstants.h"
#include "blackberrydeployconfiguration.h"
#include "blackberryrunconfigurationwidget.h"
#include "blackberrydeployinformation.h"

#include <projectexplorer/buildtargetinfo.h>
#include <projectexplorer/target.h>
#include <ssh/sshconnection.h>

#include <QFileInfo>

using namespace Qnx;
using namespace Qnx::Internal;
using namespace ProjectExplorer;

BlackBerryRunConfiguration::BlackBerryRunConfiguration(Target *parent, Core::Id id, const Utils::FileName &path)
    : RunConfiguration(parent, id)
    , m_proFilePath(path)
{
    init();
}

BlackBerryRunConfiguration::BlackBerryRunConfiguration(Target *parent,
                                                       BlackBerryRunConfiguration *source)
    : RunConfiguration(parent, source)
    , m_proFilePath(source->m_proFilePath)
{
    init();
}

void BlackBerryRunConfiguration::init()
{
    updateDisplayName();

    connect(deployConfiguration()->deploymentInfo(), SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SIGNAL(targetInformationChanged()));
    connect(deployConfiguration()->deploymentInfo(), SIGNAL(modelReset()), this, SIGNAL(targetInformationChanged()));
}

void BlackBerryRunConfiguration::updateDisplayName()
{
    if (!m_proFilePath.isEmpty())
        setDefaultDisplayName(m_proFilePath.toFileInfo().completeBaseName());
    else
        setDefaultDisplayName(tr("Run on BlackBerry device"));
}

QWidget *BlackBerryRunConfiguration::createConfigurationWidget()
{
    return new BlackBerryRunConfigurationWidget(this);
}

Utils::FileName BlackBerryRunConfiguration::proFilePath() const
{
    return m_proFilePath;
}

QString BlackBerryRunConfiguration::deviceName() const
{
    BlackBerryDeviceConfiguration::ConstPtr device
            = BlackBerryDeviceConfiguration::device(target()->kit());
    if (!device)
        return QString();

    return device->displayName();
}

Utils::FileName BlackBerryRunConfiguration::barPackage() const
{
    BlackBerryDeployConfiguration *dc = deployConfiguration();
    if (!dc)
        return Utils::FileName();

    QList<BarPackageDeployInformation> packages = dc->deploymentInfo()->enabledPackages();
    foreach (const BarPackageDeployInformation package, packages) {
        if (package.proFilePath == proFilePath())
            return package.packagePath();
    }
    return Utils::FileName();
}

QString BlackBerryRunConfiguration::localExecutableFilePath() const
{
    return target()->applicationTargets().targetForProject(m_proFilePath).toString();
}

bool BlackBerryRunConfiguration::fromMap(const QVariantMap &map)
{
    if (!RunConfiguration::fromMap(map))
        return false;

    m_proFilePath = Utils::FileName::fromUserInput(
                map.value(QLatin1String(Constants::QNX_PROFILEPATH_KEY)).toString());
    if (m_proFilePath.isEmpty() || !m_proFilePath.exists())
        return false;

    init();
    return true;
}

QVariantMap BlackBerryRunConfiguration::toMap() const
{
    QVariantMap map(RunConfiguration::toMap());
    map.insert(QLatin1String(Constants::QNX_PROFILEPATH_KEY), m_proFilePath.toString());
    return map;
}

BlackBerryDeployConfiguration *BlackBerryRunConfiguration::deployConfiguration() const
{
    return qobject_cast<BlackBerryDeployConfiguration *>(target()->activeDeployConfiguration());
}

QString BlackBerryRunConfiguration::key() const
{
    return barPackage().toString() + QLatin1Char('_')
            + BlackBerryDeviceConfiguration::device(target()->kit())->sshParameters().host;
}
