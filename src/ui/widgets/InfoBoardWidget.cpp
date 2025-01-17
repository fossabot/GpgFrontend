/**
 * This file is part of GPGFrontend.
 *
 * GPGFrontend is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Foobar is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <https://www.gnu.org/licenses/>.
 *
 * The initial version of the source code is inherited from gpg4usb-team.
 * Their source code version also complies with GNU General Public License.
 *
 * The source code version of this software was modified and released
 * by Saturneric<eric@bktus.com> starting on May 12, 2021.
 *
 */

#include "ui/widgets/InfoBoardWidget.h"

InfoBoardWidget::InfoBoardWidget(QWidget *parent, GpgME::GpgContext *ctx, KeyList *keyList) :
        QWidget(parent), mCtx(ctx), mKeyList(keyList){

    infoBoard = new QTextEdit(this);
    infoBoard->setReadOnly(true);
    infoBoard->setMinimumHeight(140);
    infoBoard->setMinimumWidth(480);

    connect(mCtx, SIGNAL(signalKeyInfoChanged()), this, SLOT(slotReset()));

    importFromKeyserverAct = new QAction(tr("Import missing key from Keyserver"), this);
    connect(importFromKeyserverAct, SIGNAL(triggered()), this, SLOT(slotImportFromKeyserver()));

    detailMenu = new QMenu(this);
    detailMenu->addAction(importFromKeyserverAct);
    importFromKeyserverAct->setVisible(false);

    actionButtonLayout = new QHBoxLayout();
    auto label = new QLabel("Optional Actions Menu");
    label->setMinimumHeight(16);
    actionButtonLayout->addWidget(label);
    actionButtonLayout->addStretch();

    QFrame *line;
    line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    auto *notificationWidgetLayout = new QVBoxLayout(this);
    notificationWidgetLayout->addWidget(infoBoard);
    notificationWidgetLayout->addLayout(actionButtonLayout);
    notificationWidgetLayout->addWidget(line);
    this->setLayout(notificationWidgetLayout);
}

void InfoBoardWidget::slotImportFromKeyserver() {
    auto *importDialog = new KeyServerImportDialog(mCtx, mKeyList, this);
    importDialog->slotImport(*keysNotInList);
}

void InfoBoardWidget::setInfoBoard(const QString &text, InfoBoardStatus verifyLabelStatus) {
    QString color;
    infoBoard->clear();
    switch (verifyLabelStatus) {
        case INFO_ERROR_OK:
            color = "#008000";
            break;
        case INFO_ERROR_WARN:
            color = "#FF8C00";
            break;
        case INFO_ERROR_CRITICAL:
            color = "#DC143C";
            break;
        default:
            break;
    }
    infoBoard->append(text);

    infoBoard->setAutoFillBackground(true);
    QPalette status = infoBoard->palette();
    status.setColor(QPalette::Text, color);
    infoBoard->setPalette(status);
    infoBoard->setFont(QFont("Times", 10));
}

void InfoBoardWidget::slotRefresh(const QString &text, InfoBoardStatus status) {
    infoBoard->clear();
    setInfoBoard(text, status);
    infoBoard->verticalScrollBar()->setValue(0);
}

void InfoBoardWidget::associateTextEdit(QTextEdit *edit) {
    this->mTextPage = edit;
    if(mTextPage != nullptr) {
        disconnect(mTextPage, SIGNAL(textChanged()), this, SLOT(slotReset()));
    }
    connect(edit, SIGNAL(textChanged()), this, SLOT(slotReset()));
}

void InfoBoardWidget::addOptionalAction(const QString& name, const std::function<void()>& action) {
    auto actionButton = new QPushButton(name);
    actionButtonLayout->addWidget(actionButton);
    connect(actionButton, &QPushButton::clicked, this, [=]() {
        action();
    });
}

void InfoBoardWidget::resetOptionActionsMenu() {
    QLayoutItem *item;
    while ((item = actionButtonLayout->layout()->takeAt( 2)) != nullptr ) {
        actionButtonLayout->removeItem(item);
        delete item->widget();
        delete item;
    }
}

void InfoBoardWidget::slotReset() {
    this->infoBoard->clear();
    resetOptionActionsMenu();
}
