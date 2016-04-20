/**************************************************************************
**
** Copyright (C) 2015 BlackBerry Limited. All rights reserved.
**
** Contact: BlackBerry (qt@blackberry.com)
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

#include "qnxconfiguration.h"
#include "qnxqtversion.h"
#include "qnxutils.h"
#include "qnxtoolchain.h"

#include "debugger/debuggeritem.h"

#include <projectexplorer/toolchainmanager.h>
#include <projectexplorer/kit.h>
#include <projectexplorer/kitmanager.h>

#include <qtsupport/baseqtversion.h>
#include <qtsupport/qtversionmanager.h>
#include <qtsupport/qtkitinformation.h>

#include <qmakeprojectmanager/qmakekitinformation.h>

#include <debugger/debuggeritemmanager.h>
#include <debugger/debuggerkitinformation.h>

#include <coreplugin/icore.h>

#include <QMessageBox>
#include <QFileInfo>

using namespace ProjectExplorer;
using namespace QtSupport;
using namespace Utils;
using namespace Debugger;

namespace Qnx {
namespace Internal {
QnxConfiguration::QnxConfiguration(const FileName &sdpEnvFile)
    : QnxBaseConfiguration(sdpEnvFile)
{
    readInformation();
}

QnxConfiguration::QnxConfiguration(const QVariantMap &data)
    : QnxBaseConfiguration(data)
{
    readInformation();
}

QString QnxConfiguration::displayName() const
{
    return m_configName;
}

bool QnxConfiguration::activate()
{
    if (isActive())
        return true;

    if (!isValid()) {
        QString errorMessage = tr("The following errors occurred while activating the QNX configuration:");
        foreach (const QString &error, validationErrors())
            errorMessage += QLatin1String("\n") + error;

        QMessageBox::warning(Core::ICore::mainWindow(), tr("Cannot Set Up QNX Configuration"),
                             errorMessage, QMessageBox::Ok);
        return false;
    }

    // Create and register toolchain
    QnxToolChain *armTc = createToolChain(ArmLeV7,
                    tr("QCC for %1 (armv7)").arg(displayName()),
                    sdpPath().toString());
    QnxToolChain *x86Tc = createToolChain(X86,
                    tr("QCC for %1 (x86)").arg(displayName()),
                    sdpPath().toString());

    // Create and register debuggers
    QVariant armDebuggerId = createDebuggerItem(ArmLeV7,
                       tr("Debugger for %1 (armv7)").arg(displayName()));

    QVariant x86DebuggerId = createDebuggerItem(X86,
                       tr("Debugger for %1 (x86)").arg(displayName()));

    // Create and register kits
    createKit(ArmLeV7, armTc, armDebuggerId, tr("Kit for %1 (armv7)").arg(displayName()));
    createKit(X86, x86Tc, x86DebuggerId, tr("Kit for %1 (x86)").arg(displayName()));

    return true;
}

void QnxConfiguration::deactivate()
{
    if (!isActive())
        return;

    QList<ToolChain *> toolChainsToRemove;
    QList<DebuggerItem> debuggersToRemove;
    foreach (ToolChain *tc,
             ToolChainManager::toolChains()) {
        if (tc->compilerCommand() == qccCompilerPath())
            toolChainsToRemove.append(tc);
    }

    foreach (DebuggerItem debuggerItem,
             DebuggerItemManager::debuggers()) {
        if (debuggerItem.command() == armDebuggerPath() ||
                debuggerItem.command() == x86DebuggerPath())
            debuggersToRemove.append(debuggerItem);
    }

    foreach (Kit *kit, KitManager::kits()) {
        if (kit->isAutoDetected()
                && DeviceTypeKitInformation::deviceTypeId(kit) == Constants::QNX_BB_OS_TYPE
                && toolChainsToRemove.contains(ToolChainKitInformation::toolChain(kit)))
            KitManager::deregisterKit(kit);
    }

    foreach (ToolChain *tc, toolChainsToRemove)
        ToolChainManager::deregisterToolChain(tc);

    foreach (DebuggerItem debuggerItem, debuggersToRemove)
        DebuggerItemManager::
                deregisterDebugger(debuggerItem.id());
}

bool QnxConfiguration::isActive() const
{
    bool hasToolChain = false;
    bool hasDebugger = false;
    foreach (ToolChain *tc,
             ToolChainManager::toolChains()) {
        if (tc->compilerCommand() == qccCompilerPath()) {
            hasToolChain = true;
            break;
        }
    }

    foreach (DebuggerItem debuggerItem,
             DebuggerItemManager::debuggers()) {
        if (debuggerItem.command() == armDebuggerPath() ||
                debuggerItem.command() == x86DebuggerPath()) {
            hasDebugger = true;
            break;
        }
    }

    return hasToolChain && hasDebugger;
}

bool QnxConfiguration::canCreateKits() const
{
    return isValid() && (qnxQtVersion(ArmLeV7) || qnxQtVersion(X86));
}

FileName QnxConfiguration::sdpPath() const
{
    return envFile().parentDir();
}

QnxQtVersion* QnxConfiguration::qnxQtVersion(QnxArchitecture arch) const
{
    QnxQtVersion *qnxQt;
    foreach (BaseQtVersion *version,
             QtVersionManager::instance()->versions()) {
        if (version->type() == QLatin1String(Constants::QNX_QNX_QT)) {
            qnxQt = dynamic_cast<QnxQtVersion*>(version);
            if (qnxQt && qnxQt->architecture() == arch) {
                return qnxQt;
            }
        }
    }

    return 0;
}

Kit *QnxConfiguration::createKit(QnxArchitecture arch,
                                                  QnxToolChain *toolChain,
                                                  const QVariant &debuggerItemId,
                                                  const QString &displayName)
{
    QnxQtVersion *qnxQt = qnxQtVersion(arch);
    // Do not create incomplete kits if no qt qnx version found
    if (!qnxQt)
        return 0;

    Kit *kit = new Kit;

    QtKitInformation::setQtVersion(kit, qnxQt);
    ToolChainKitInformation::setToolChain(kit, toolChain);

    if (debuggerItemId.isValid())
        DebuggerKitInformation::setDebugger(kit, debuggerItemId);

    if (arch == X86) {
        QmakeProjectManager::QmakeKitInformation::setMkspec(
                    kit, FileName::fromLatin1("qnx-x86-qcc"));
    } else {
        QmakeProjectManager::QmakeKitInformation::setMkspec(
                    kit, FileName::fromLatin1("qnx-armv7le-qcc"));
    }

    DeviceTypeKitInformation::setDeviceTypeId(kit, Constants::QNX_QNX_OS_TYPE);
    // TODO: Add sysroot?

    kit->setUnexpandedDisplayName(displayName);
    kit->setIconPath(FileName::fromString(QLatin1String(Constants::QNX_CATEGORY_ICON)));

    kit->setAutoDetected(true);
    kit->setAutoDetectionSource(envFile().toString());
    kit->setMutable(DeviceKitInformation::id(), true);

    kit->setSticky(ToolChainKitInformation::id(), true);
    kit->setSticky(DeviceTypeKitInformation::id(), true);
    kit->setSticky(SysRootKitInformation::id(), true);
    kit->setSticky(DebuggerKitInformation::id(), true);
    kit->setSticky(QmakeProjectManager::QmakeKitInformation::id(), true);

    // add kit with device and qt version not sticky
    KitManager::registerKit(kit);
    return kit;
}

void QnxConfiguration::readInformation()
{
    QString qConfigPath = sdpPath().toString() + QLatin1String("/.qnx/qconfig");
    QList <ConfigInstallInformation> installInfoList = QnxUtils::installedConfigs(qConfigPath);
    if (installInfoList.isEmpty())
        return;

    // TODO: For now (6.6) it should be one installation file. The code need to handle cases
    // where the SDP support many target/host installations (i.e many installation files).
    const ConfigInstallInformation installInfo = installInfoList.first();
    m_configName = installInfo.name;
    setVersion(QnxVersionNumber(installInfo.version));
}

}
}
