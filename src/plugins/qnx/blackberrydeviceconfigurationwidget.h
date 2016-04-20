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

#ifndef QNX_INTERNAL_BLACKBERRYDEVICECONFIGURATIONWIDGET_H
#define QNX_INTERNAL_BLACKBERRYDEVICECONFIGURATIONWIDGET_H

#include <projectexplorer/devicesupport/idevicewidget.h>

#include "blackberrydeviceconfiguration.h"

QT_BEGIN_NAMESPACE
class QProgressDialog;
class QAbstractButton;
QT_END_NAMESPACE

namespace Qnx {
namespace Internal {

class BlackBerryDebugTokenUploader;
class BlackBerrySigningUtils;

namespace Ui { class BlackBerryDeviceConfigurationWidget; }

class BlackBerryDeviceConfigurationWidget : public ProjectExplorer::IDeviceWidget
{
    Q_OBJECT

public:
    explicit BlackBerryDeviceConfigurationWidget(const ProjectExplorer::IDevice::Ptr &device,
                                          QWidget *parent = 0);
    ~BlackBerryDeviceConfigurationWidget();

private slots:
    void hostNameEditingFinished();
    void passwordEditingFinished();
    void keyFileEditingFinished();
    void showPassword(bool showClearText);
    void debugTokenEditingFinished();
    void importDebugToken();
    void requestDebugToken();
    void uploadDebugToken();
    void updateUploadButton();
    void uploadFinished(int status);
    void appendConnectionLog(Core::Id deviceId, const QString &line);
    void clearConnectionLog(Core::Id deviceId);
    void populateDebugTokenCombo(const QString &current);
    void updateDebugTokenCombo();

private:
    void updateDeviceFromUi();
    void initGui();

    BlackBerryDeviceConfiguration::Ptr deviceConfiguration() const;

    Ui::BlackBerryDeviceConfigurationWidget *ui;
    QAbstractButton *uploadButton;

    QProgressDialog *progressDialog;

    BlackBerryDebugTokenUploader *uploader;
    BlackBerrySigningUtils &m_utils;
};


} // namespace Internal
} // namespace Qnx

#endif // QNX_INTERNAL_BLACKBERRYDEVICECONFIGURATIONWIDGET_H
