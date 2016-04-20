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

#include "blackberryimportcertificatedialog.h"
#include "blackberrycertificate.h"
#include "ui_blackberryimportcertificatedialog.h"

#include <QPushButton>
#include <QMessageBox>

#include <utils/pathchooser.h>

namespace Qnx {
namespace Internal {

BlackBerryImportCertificateDialog::BlackBerryImportCertificateDialog(
        QWidget *parent, Qt::WindowFlags f) :
    QDialog(parent, f),
    m_ui(new Ui_BlackBerryImportCertificateDialog),
    m_certificate(0)
{
    m_ui->setupUi(this);
    m_ui->certPath->setExpectedKind(Utils::PathChooser::File);
    m_ui->certPath->setHistoryCompleter(QLatin1String("BB.Certificate.History"));
    m_ui->certPath->setPromptDialogTitle(tr("Import Certificate"));
    m_ui->certPath->setPromptDialogFilter(tr("PKCS 12 Archives (*.p12)"));

    m_cancelButton = m_ui->buttonBox->button(QDialogButtonBox::Cancel);

    m_okButton = m_ui->buttonBox->button(QDialogButtonBox::Ok);
    m_okButton->setEnabled(false);

    connect(m_cancelButton, SIGNAL(clicked()),
            this, SLOT(reject()));
    connect(m_okButton, SIGNAL(clicked()),
            this, SLOT(importCertificate()));
    connect(m_ui->certPath, SIGNAL(changed(QString)),
            this, SLOT(validate()));
    connect(m_ui->certPass, SIGNAL(textChanged(QString)),
            this, SLOT(validate()));
}

QString BlackBerryImportCertificateDialog::author() const
{
    return m_author;
}

QString BlackBerryImportCertificateDialog::certPath() const
{
    return m_ui->certPath->path();
}

QString BlackBerryImportCertificateDialog::keystorePassword() const
{
    return m_ui->certPass->text();
}

BlackBerryCertificate * BlackBerryImportCertificateDialog::certificate() const
{
    return m_certificate;
}

void BlackBerryImportCertificateDialog::importCertificate()
{
    setBusy(true);

    m_certificate = new BlackBerryCertificate(certPath(),
            QString(), keystorePassword());

    connect(m_certificate, SIGNAL(finished(int)), this, SLOT(certificateLoaded(int)));

    m_certificate->load();
}

void BlackBerryImportCertificateDialog::validate()
{
    if (!m_ui->certPath->isValid() || m_ui->certPass->text().isEmpty()) {
        m_okButton->setEnabled(false);
        return;
    }

    m_okButton->setEnabled(true);
}

void BlackBerryImportCertificateDialog::certificateLoaded(int status)
{
    if (status != BlackBerryCertificate::Success) {
        setBusy(false);

        m_certificate->deleteLater();
        m_certificate = 0;

        QString message;

        if (status == BlackBerryCertificate::WrongPassword)
            message = tr("The keystore password is invalid.");
        else if (status == BlackBerryCertificate::InvalidOutputFormat)
            message = tr("Error parsing inferior process output.");
        else
            message = tr("An unknown error has occurred.");

        QMessageBox::information(this, tr("Error"), message);

    } else {
        m_author = m_certificate->author();
        accept();
    }
}

void BlackBerryImportCertificateDialog::setBusy(bool busy)
{
    m_ui->certPath->setEnabled(!busy);
    m_ui->certPass->setEnabled(!busy);
    m_okButton->setEnabled(!busy);
    m_cancelButton->setEnabled(!busy);
}

} // namespace Internal
} // namespace Qnx
