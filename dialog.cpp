/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include "dialog.h"

int uniqueAlbumId;
int uniqueArtistId;

Dialog::Dialog(QSqlRelationalTableModel *hours, QWidget *parent)
     : QDialog(parent)
{
    model = hours;

    QGroupBox *inputWidgetBox = createInputWidgets();
    QDialogButtonBox *buttonBox = createButtons();

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(inputWidgetBox);
    layout->addWidget(buttonBox);
    setLayout(layout);

    setWindowTitle(tr("Add record"));
}

void Dialog::submit()
{
    int albumId = addNewRecord();
}

int Dialog::addNewRecord()
{
    int id = generateRecordId();
    QSqlRecord record;

    QSqlField f1("id", QVariant::Int);
    QSqlField f2("day", QVariant::UInt);
    QSqlField f3("start", QVariant::Int);
    QSqlField f4("end", QVariant::Int);

    f1.setValue(QVariant(id));
	QDateTime currentDate = QDateTime(QDate::currentDate());
    f2.setValue(QVariant(currentDate.toTime_t()));
    f3.setValue(QVariant(QDateTime::currentDateTime().toTime_t()));
    f4.setValue(QVariant(0));
    record.append(f1);
    record.append(f2);
    record.append(f3);
    record.append(f4);

    model->insertRecord(-1, record);

    return id;
}

void Dialog::revert()
{
    artistEditor->clear();
    titleEditor->clear();
    yearEditor->setValue(QDate::currentDate().year());
    tracksEditor->clear();
}

QGroupBox *Dialog::createInputWidgets()
{
    QGroupBox *box = new QGroupBox(tr("Add Album"));

    QLabel *artistLabel = new QLabel(tr("Artist:"));
    QLabel *titleLabel = new QLabel(tr("Title:"));
    QLabel *yearLabel = new QLabel(tr("Year:"));
    QLabel *tracksLabel = new QLabel(tr("Tracks (separated by comma):"));

    artistEditor = new QLineEdit;
    titleEditor = new QLineEdit;

    yearEditor = new QSpinBox;
    yearEditor->setMinimum(1900);
    yearEditor->setMaximum(QDate::currentDate().year());
    yearEditor->setValue(yearEditor->maximum());
    yearEditor->setReadOnly(false);

    tracksEditor = new QLineEdit;

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(artistLabel, 0, 0);
    layout->addWidget(artistEditor, 0, 1);
    layout->addWidget(titleLabel, 1, 0);
    layout->addWidget(titleEditor, 1, 1);
    layout->addWidget(yearLabel, 2, 0);
    layout->addWidget(yearEditor, 2, 1);
    layout->addWidget(tracksLabel, 3, 0, 1, 2);
    layout->addWidget(tracksEditor, 4, 0, 1, 2);
    box->setLayout(layout);

    return box;
}

QDialogButtonBox *Dialog::createButtons()
{
    QPushButton *closeButton = new QPushButton(tr("&Close"));
    QPushButton *revertButton = new QPushButton(tr("&Revert"));
    QPushButton *submitButton = new QPushButton(tr("&Submit"));

    closeButton->setDefault(true);

    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(revertButton, SIGNAL(clicked()), this, SLOT(revert()));
    connect(submitButton, SIGNAL(clicked()), this, SLOT(submit()));

    QDialogButtonBox *buttonBox = new QDialogButtonBox;
    buttonBox->addButton(submitButton, QDialogButtonBox::ResetRole);
    buttonBox->addButton(revertButton, QDialogButtonBox::ResetRole);
    buttonBox->addButton(closeButton, QDialogButtonBox::RejectRole);

    return buttonBox;
}

int Dialog::generateRecordId()
{
    uniqueAlbumId += 1;
    return uniqueAlbumId;
}
