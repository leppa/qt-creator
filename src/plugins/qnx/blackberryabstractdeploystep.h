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

#ifndef QNX_INTERNAL_BLACKBERRYABSTRACTDEPLOYSTEP_H
#define QNX_INTERNAL_BLACKBERRYABSTRACTDEPLOYSTEP_H

#include "blackberryprocessparser.h"

#include <projectexplorer/buildstep.h>
#include <projectexplorer/processparameters.h>

#include <QProcess>

QT_BEGIN_NAMESPACE
class QEventLoop;
QT_END_NAMESPACE

namespace Utils { class QtcProcess; }

namespace Qnx {
namespace Internal {

class BlackBerryAbstractDeployStep : public ProjectExplorer::BuildStep
{
    Q_OBJECT
public:
    explicit BlackBerryAbstractDeployStep(ProjectExplorer::BuildStepList *bsl, Core::Id id);
    ~BlackBerryAbstractDeployStep();

    bool init();
    void run(QFutureInterface<bool> &fi);

protected:
    BlackBerryAbstractDeployStep(ProjectExplorer::BuildStepList *bsl, BlackBerryAbstractDeployStep *bs);

    void addCommand(const QString &command, const QStringList &arguments);

    virtual void stdOutput(const QString &line);
    virtual void stdError(const QString &line);

    virtual void processStarted(const ProjectExplorer::ProcessParameters &param);

    void emitOutputInfo(const ProjectExplorer::ProcessParameters &params, const QString& arguments);

    void raiseError(const QString &errorMessage);

private slots:
    void reportProgress(int progress);

    void processReadyReadStdOutput();
    void processReadyReadStdError();

    void checkForCancel();

    void handleProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
    void ctor();

    void runCommands();
    void runNextCommand();

    QList<ProjectExplorer::ProcessParameters> m_params;
    int m_processCounter;
    Utils::QtcProcess *m_process;

    Utils::Environment m_environment;
    QString m_buildDirectory;

    QTimer *m_timer;
    QFutureInterface<bool> *m_futureInterface;
    QEventLoop *m_eventLoop;

    BlackBerryProcessParser m_outputParser;
};

} // namespace Internal
} // namespace Qnx

#endif // QNX_INTERNAL_BLACKBERRYABSTRACTDEPLOYSTEP_H
