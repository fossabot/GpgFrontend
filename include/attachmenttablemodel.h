/*
 *      attachmenttablemodel.h
 *
 *      Copyright 2008 gpg4usb-team <gpg4usb@cpunk.de>
 *
 *      This file is part of gpg4usb.
 *
 *      Gpg4usb is free software: you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation, either version 3 of the License, or
 *      (at your option) any later version.
 *
 *      Gpg4usb is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with gpg4usb.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef __ATTACHMENTTABLEMODEL_H__
#define __ATTACHMENTTABLEMODEL_H__

#include "mime.h"

class AttachmentTableModel : public QAbstractTableModel {
Q_OBJECT

public:
    explicit AttachmentTableModel(QObject *parent = nullptr);

    explicit AttachmentTableModel(QList<MimePart> mimeparts, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent) const override;

    int columnCount(const QModelIndex &parent) const override;

    QVariant data(const QModelIndex &index, int role) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    void add(const MimePart& mp);

    MimePart getSelectedMimePart(QModelIndex index);

    MimePart getMimePart(int index);
    //QList<MimePart> getSelectedMimeParts(QModelIndexList indexes);

private:
    QList<MimePart> listOfMimeparts;
};

#endif // __ATTACHMENTTABLEMODEL_H__
