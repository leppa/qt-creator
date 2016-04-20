/**************************************************************************
**
** Copyright (C) 2015 BlackBerry Limited. All rights reserved.
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

#ifndef BLACKBERRYCONFIGURATIONS_H
#define BLACKBERRYCONFIGURATIONS_H

#include "qnxutils.h"
#include "qnxversionnumber.h"
#include "qnxbaseconfiguration.h"
#include "qnxconstants.h"

#include <utils/environment.h>
#include <utils/fileutils.h>

#include <projectexplorer/abi.h>
#include <projectexplorer/kit.h>

#include <QObject>
#include <QCoreApplication>

namespace QtSupport { class BaseQtVersion; }
namespace Debugger { class DebuggerItem; }

namespace Qnx {
namespace Internal {

class QnxAbstractQtVersion;
class QnxToolChain;

class BlackBerryApiLevelConfiguration : public QnxBaseConfiguration
{
    Q_DECLARE_TR_FUNCTIONS(Qnx::Internal::BlackBerryApiLevelConfiguration)
public:
    BlackBerryApiLevelConfiguration(const ConfigInstallInformation &ndkInstallInfo);
    BlackBerryApiLevelConfiguration(const Utils::FileName &ndkEnvFile);
    BlackBerryApiLevelConfiguration(const QVariantMap &data);
    bool activate();
    void deactivate();
    QString ndkPath() const;
    QString displayName() const;
    QString targetName() const;
    bool isAutoDetected() const;
    Utils::FileName autoDetectionSource() const;
    bool isActive() const;
    bool isValid() const;
    Utils::FileName qmake4BinaryFile() const;
    Utils::FileName qmake5BinaryFile() const;
    Utils::FileName sysRoot() const;
    QVariantMap toMap() const;

#ifdef WITH_TESTS
    static void setFakeConfig(bool fakeConfig);
    static bool fakeConfig();
#endif

protected:
    QnxAbstractQtVersion *createQtVersion(
            const Utils::FileName &qmakePath, Qnx::QnxArchitecture arch, const QString &versionName);
    ProjectExplorer::Kit *createKit(
            QnxAbstractQtVersion *version, QnxToolChain *armToolChain, const QVariant &debuggerItemId);
    QStringList validationErrors() const;

private:
    QString m_displayName;
    QString m_targetName;
    Utils::FileName m_autoDetectionSource;
    Utils::FileName m_qmake4BinaryFile;
    Utils::FileName m_qmake5BinaryFile;
    Utils::FileName m_sysRoot;

    void ctor();

#ifdef WITH_TESTS
    static bool m_fakeConfig;
#endif
};

} // namespace Internal
} // namespace Qnx

#endif // BLACKBERRYCONFIGURATIONS_H
