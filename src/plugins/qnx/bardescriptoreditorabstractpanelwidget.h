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

#ifndef QNX_INTERNAL_BARDESCRIPTOREDITORABSTRACTPANELWIDGET_H
#define QNX_INTERNAL_BARDESCRIPTOREDITORABSTRACTPANELWIDGET_H

#include <QWidget>

#include "bardescriptordocument.h"

namespace Utils { class PathChooser; }

QT_BEGIN_NAMESPACE
class QCheckBox;
class QComboBox;
class QLineEdit;
class QSignalMapper;
class QStringListModel;
class QTextEdit;
QT_END_NAMESPACE

namespace Qnx {
namespace Internal {

class BarDescriptorEditorAbstractPanelWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BarDescriptorEditorAbstractPanelWidget(QWidget *parent = 0);

public slots:
    void setValue(BarDescriptorDocument::Tag tag, const QVariant &value);

signals:
    void changed(BarDescriptorDocument::Tag tag, const QVariant &value);

protected:
    virtual void updateWidgetValue(BarDescriptorDocument::Tag tag, const QVariant &value);
    virtual void emitChanged(BarDescriptorDocument::Tag tag);

    void addSignalMapping(BarDescriptorDocument::Tag tag, QObject *object, const char *signal);
    void blockSignalMapping(BarDescriptorDocument::Tag tag);
    void unblockSignalMapping(BarDescriptorDocument::Tag tag);

private slots:
    void handleSignalMapped(int id);

private:
    QSignalMapper *m_signalMapper;
    QList<BarDescriptorDocument::Tag> m_blockedSignals;
};

} // namespace Internal
} // namespace Qnx

#endif // QNX_INTERNAL_BARDESCRIPTOREDITORABSTRACTPANELWIDGET_H
