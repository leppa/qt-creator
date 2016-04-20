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

#include "blackberryruntimeconfiguration.h"

#include "qnxconstants.h"

#include <QVariantMap>
#include <QFileInfo>
#include <QCoreApplication>

namespace Qnx {
namespace Internal {

const QLatin1String PathKey("Path");
const QLatin1String DisplayNameKey("DisplayName");
const QLatin1String VersionKey("Version");

BlackBerryRuntimeConfiguration::BlackBerryRuntimeConfiguration(
        const QString &path,
        const QnxVersionNumber &version)
    : m_path(path)
{
    if (!version.isEmpty())
        m_version = version;
    else
        m_version = QnxVersionNumber::fromFileName(QFileInfo(path).baseName(),
                                                          QRegExp(QLatin1String("^runtime_(.*)$")));

    m_displayName = QCoreApplication::translate("Qnx::Internal::BlackBerryRuntimeConfiguration", "Runtime %1").arg(m_version.toString());
}

BlackBerryRuntimeConfiguration::BlackBerryRuntimeConfiguration(const QVariantMap &data)
{
    m_path = data.value(QLatin1String(PathKey)).toString();
    m_displayName = data.value(QLatin1String(DisplayNameKey)).toString();
    m_version = QnxVersionNumber(data.value(QLatin1String(VersionKey)).toString());
}

QString BlackBerryRuntimeConfiguration::path() const
{
    return m_path;
}

QString BlackBerryRuntimeConfiguration::displayName() const
{
    return m_displayName;
}

QnxVersionNumber BlackBerryRuntimeConfiguration::version() const
{
    return m_version;
}

QVariantMap BlackBerryRuntimeConfiguration::toMap() const
{
    QVariantMap data;
    data.insert(QLatin1String(Qnx::Constants::QNX_BB_KEY_CONFIGURATION_TYPE),
                QLatin1String(Qnx::Constants::QNX_BB_RUNTIME_TYPE));
    data.insert(QLatin1String(PathKey), m_path);
    data.insert(QLatin1String(DisplayNameKey), m_displayName);
    data.insert(QLatin1String(VersionKey), m_version.toString());
    return data;
}

}
}
