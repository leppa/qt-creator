/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing
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
#ifndef SHELL_H
#define SHELL_H

#include <QObject>
#include <QSharedPointer>

namespace QSsh {
class SshConnection;
class SshConnectionParameters;
class SshRemoteProcess;
}

QT_BEGIN_NAMESPACE
class QByteArray;
class QFile;
class QString;
QT_END_NAMESPACE

class Shell : public QObject
{
    Q_OBJECT
public:
    Shell(const QSsh::SshConnectionParameters &parameters, QObject *parent = 0);
    ~Shell();

    void run();

private slots:
    void handleConnected();
    void handleConnectionError();
    void handleRemoteStdout();
    void handleRemoteStderr();
    void handleShellMessage(const QString &message);
    void handleChannelClosed(int exitStatus);
    void handleShellStarted();
    void handleStdin();

private:
    QSsh::SshConnection *m_connection;
    QSharedPointer<QSsh::SshRemoteProcess> m_shell;
    QFile * const m_stdin;
};

#endif // SHELL_H
