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

#ifndef QNX_INTERNAL_BLACKBERRYRUNCONFIGURATION_H
#define QNX_INTERNAL_BLACKBERRYRUNCONFIGURATION_H

#include <projectexplorer/runconfiguration.h>

#include <utils/fileutils.h>

namespace ProjectExplorer { class Target; }

namespace QmakeProjectManager {
class QmakeBuildConfiguration;
class QmakeProject;
}

namespace Qnx {
namespace Internal {

class BlackBerryDeployConfiguration;

class BlackBerryRunConfiguration : public ProjectExplorer::RunConfiguration
{
    Q_OBJECT
    friend class BlackBerryRunConfigurationFactory;

public:
    explicit BlackBerryRunConfiguration(ProjectExplorer::Target *parent, Core::Id id,
                                        const Utils::FileName &path);

    QWidget *createConfigurationWidget();

    Utils::FileName proFilePath() const;

    QString deviceName() const;
    Utils::FileName barPackage() const;

    QString localExecutableFilePath() const;

    bool fromMap(const QVariantMap &map);
    QVariantMap toMap() const;

    BlackBerryDeployConfiguration *deployConfiguration() const;
    QmakeProjectManager::QmakeBuildConfiguration *activeQmakeBuildConfiguration() const;

    QString key() const;

signals:
    void targetInformationChanged();

protected:
    BlackBerryRunConfiguration(ProjectExplorer::Target *parent, BlackBerryRunConfiguration *source);

private:
    void init();
    void updateDisplayName();

    Utils::FileName m_proFilePath;
};

} // namespace Internal
} // namespace Qnx

#endif // QNX_INTERNAL_BLACKBERRYRUNCONFIGURATION_H
