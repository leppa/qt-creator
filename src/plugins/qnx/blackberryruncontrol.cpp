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

#include "blackberryruncontrol.h"
#include "blackberryapplicationrunner.h"
#include "blackberryrunconfiguration.h"
#include "blackberrydeviceconnectionmanager.h"

#include <QIcon>
#include <QTimer>

using namespace Qnx;
using namespace Qnx::Internal;

BlackBerryRunControl::BlackBerryRunControl(BlackBerryRunConfiguration *runConfiguration)
    : ProjectExplorer::RunControl(runConfiguration, ProjectExplorer::NormalRunMode)
{
    setIcon(QLatin1String(ProjectExplorer::Constants::ICON_RUN_SMALL));

    m_runner = new BlackBerryApplicationRunner(BlackBerryApplicationRunner::LaunchFlags(), runConfiguration, this);

    connect(m_runner, SIGNAL(started()), this, SIGNAL(started()));
    connect(m_runner, SIGNAL(finished()), this, SIGNAL(finished()));
    connect(m_runner, SIGNAL(output(QString,Utils::OutputFormat)),
            this, SLOT(appendMessage(QString,Utils::OutputFormat)));
    connect(m_runner, SIGNAL(startFailed(QString)), this, SLOT(handleStartFailed(QString)));
}

void BlackBerryRunControl::start()
{
    m_runner->start();
}

ProjectExplorer::RunControl::StopResult BlackBerryRunControl::stop()
{
    return m_runner->stop();
}

bool BlackBerryRunControl::isRunning() const
{
    return m_runner->isRunning();
}

void BlackBerryRunControl::handleStartFailed(const QString &message)
{
    appendMessage(message, Utils::StdErrFormat);
}
