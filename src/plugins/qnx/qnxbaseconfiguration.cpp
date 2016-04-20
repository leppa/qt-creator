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

#include "qnxbaseconfiguration.h"
#include "qnxutils.h"
#include "qnxtoolchain.h"

#include <projectexplorer/toolchain.h>
#include <projectexplorer/toolchainmanager.h>

#include <debugger/debuggerkitinformation.h>
#include <debugger/debuggeritem.h>

#include <coreplugin/icore.h>

#include <QVariantMap>
#include <QFileInfo>
#include <QMessageBox>

namespace Qnx {
namespace Internal {

const QLatin1String QNXEnvFileKey("EnvFile");
const QLatin1String QNXVersionKey("QNXVersion");
// For backward compatibility
const QLatin1String NDKEnvFileKey("NDKEnvFile");


using namespace Utils;
using namespace ProjectExplorer;

QnxBaseConfiguration::QnxBaseConfiguration()
{
}

QnxBaseConfiguration::QnxBaseConfiguration(const FileName &envFile)
{
    ctor(envFile);
}

QnxBaseConfiguration::QnxBaseConfiguration(const QVariantMap &data)
{
    QString envFilePath = data.value(QNXEnvFileKey).toString();
    if (envFilePath.isEmpty())
        envFilePath = data.value(NDKEnvFileKey).toString();

    m_version = QnxVersionNumber(data.value(QNXVersionKey).toString());
    ctor(FileName::fromString(envFilePath));
}

QnxBaseConfiguration::~QnxBaseConfiguration()
{
}

FileName QnxBaseConfiguration::envFile() const
{
    return m_envFile;
}

FileName QnxBaseConfiguration::qnxTarget() const
{
    return m_qnxTarget;
}

FileName QnxBaseConfiguration::qnxHost() const
{
    return m_qnxHost;
}

FileName QnxBaseConfiguration::qccCompilerPath() const
{
    return m_qccCompiler;
}

FileName QnxBaseConfiguration::armDebuggerPath() const
{
    return m_armlev7Debugger;
}

FileName QnxBaseConfiguration::x86DebuggerPath() const
{
    return m_x86Debugger;
}

QList<EnvironmentItem> QnxBaseConfiguration::qnxEnv() const
{
    return m_qnxEnv;
}

QnxVersionNumber QnxBaseConfiguration::version() const
{
    return m_version;
}

QVariantMap QnxBaseConfiguration::toMap() const
{
    QVariantMap data;
    data.insert(QLatin1String(QNXEnvFileKey), m_envFile.toString());
    data.insert(QLatin1String(QNXVersionKey), m_version.toString());
    return data;
}

bool QnxBaseConfiguration::isValid() const
{
    return !m_qccCompiler.isEmpty()
            && !m_armlev7Debugger.isEmpty()
            && !m_x86Debugger.isEmpty();
}

void QnxBaseConfiguration::ctor(const FileName &envScript)
{
    if (envScript.isEmpty())
        return;

#if !defined(WITH_TESTS)
    QTC_ASSERT(envScript.exists(), return);
#endif
    m_envFile = envScript;
    m_qnxEnv = QnxUtils::qnxEnvironmentFromEnvFile(m_envFile.toString());
    foreach (const EnvironmentItem &item, m_qnxEnv) {
        if (item.name == QLatin1String("QNX_TARGET"))
            m_qnxTarget = FileName::fromString(item.value);

        else if (item.name == QLatin1String("QNX_HOST"))
            m_qnxHost = FileName::fromString(item.value);
    }

    FileName qccPath = FileName::fromString(HostOsInfo::withExecutableSuffix(
        m_qnxHost.toString() + QLatin1String("/usr/bin/qcc")));
    FileName armlev7GdbPath = FileName::fromString(HostOsInfo::withExecutableSuffix(
        m_qnxHost.toString() + QLatin1String("/usr/bin/ntoarm-gdb")));
    if (!armlev7GdbPath.exists()) {
        armlev7GdbPath = FileName::fromString(HostOsInfo::withExecutableSuffix(
            m_qnxHost.toString() + QLatin1String("/usr/bin/ntoarmv7-gdb")));
    }

    FileName x86GdbPath = FileName::fromString(HostOsInfo::withExecutableSuffix(
        m_qnxHost.toString() + QLatin1String("/usr/bin/ntox86-gdb")));

    if (qccPath.exists())
        m_qccCompiler = qccPath;

    if (armlev7GdbPath.exists())
        m_armlev7Debugger = armlev7GdbPath;

    if (x86GdbPath.exists())
        m_x86Debugger = x86GdbPath;
}

QVariant QnxBaseConfiguration::createDebuggerItem(QnxArchitecture arch,
                                              const QString &displayName)
{
    FileName command = (arch == X86) ? x86DebuggerPath() : armDebuggerPath();
    Debugger::DebuggerItem debugger;
    debugger.setCommand(command);
    debugger.setEngineType(Debugger::GdbEngineType);
    debugger.setAbi(Abi(arch == Qnx::ArmLeV7 ? Abi::ArmArchitecture : Abi::X86Architecture,
                        Abi::LinuxOS, Abi::GenericLinuxFlavor, Abi::ElfFormat, 32));
    debugger.setAutoDetected(true);
    debugger.setUnexpandedDisplayName(displayName);
    return Debugger::DebuggerItemManager::registerDebugger(debugger);
}

QnxToolChain *QnxBaseConfiguration::createToolChain(QnxArchitecture arch,
                                                    const QString &displayName,
                                                    const QString &ndkPath)
{
    QnxToolChain *toolChain = new QnxToolChain(ToolChain::AutoDetection);
    toolChain->resetToolChain(m_qccCompiler);
    toolChain->setTargetAbi(Abi((arch == Qnx::ArmLeV7) ? Abi::ArmArchitecture : Abi::X86Architecture,
                                Abi::LinuxOS, Abi::GenericLinuxFlavor, Abi::ElfFormat, 32));
    toolChain->setDisplayName(displayName);
    toolChain->setNdkPath(ndkPath);
    ToolChainManager::registerToolChain(toolChain);
    return toolChain;
}

QStringList QnxBaseConfiguration::validationErrors() const
{
    QStringList errorStrings;
    if (m_qccCompiler.isEmpty())
        errorStrings << tr("- No GCC compiler found.");

    if (m_armlev7Debugger.isEmpty())
        errorStrings << tr("- No GDB debugger found for armvle7.");

    if (m_x86Debugger.isEmpty())
        errorStrings << tr("- No GDB debugger found for x86.");

    return errorStrings;
}

void QnxBaseConfiguration::setVersion(const QnxVersionNumber &version)
{
    m_version = version;
}

}
}
