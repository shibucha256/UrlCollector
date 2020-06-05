/*
 *  mainwindow.cpp: Implementation of mainwindow.h
 *
 * Copyright (C) 2020 Shibucha <shibucha256@gmail.com>
 *
 * This file is part of UrlCollector.
 *
 * UrlCollector is free software: you can redistribute it and/or modify it under the terms
 * of the GNU Lesser General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * UrlCollector is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU  Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License along with
 * UrlCollector. If not, see <http://www.gnu.org/licenses/>.
 * */

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QClipboard>
#include <QMimeData>
#include <QUrl>
#include <QListWidgetItem>
//#include <QtDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(QApplication::clipboard(), &QClipboard::dataChanged, this, &MainWindow::clipBoardChanged);
    connect(this, &MainWindow::textFound, this, &MainWindow::textArrived, Qt::QueuedConnection);
}

MainWindow::~MainWindow()
{
    disconnect(this, &MainWindow::textFound, this, &MainWindow::textArrived);
    disconnect(QApplication::clipboard(), &QClipboard::dataChanged, this, &MainWindow::clipBoardChanged);

    delete ui;
}

void MainWindow::clipBoardChanged()
{

    const QMimeData *mimeData = QApplication::clipboard()->mimeData();

    if (mimeData->hasText()) {

        const QString text = mimeData->text();
        emit textFound(text);
    }
    else if (mimeData->hasHtml()) {

        const QString text = mimeData->html();
        emit textFound(text);
    }
    else if (mimeData->hasUrls()) {

        const QList<QUrl> urls = mimeData->urls();

        for(auto it = urls.begin(); it != urls.end(); it++) {

            const QUrl& url = *it;
            const QString& text = url.url();
            emit textFound(text);
        }
    }

}

void MainWindow::textArrived(const QString& text)
{
    const QStringList lines = text.split(QRegularExpression("(\\r|\\n|\\r\\n)"));

    // extract only url.
//    const QRegularExpression re1("(?<url>((ftp|https?)://\\S+)|(~?/\\S+))");      // ftp http https ~/ /
//    const QRegularExpression re2("(^|[^:])(?<url>\\\\\\\\[^\\\\\\s]\\S+)");       // windows unc
//    const QRegularExpression re3("(?<url>(([a-zA-Z]:\\\\[^\\s]+)|(C:\\\\Program Files\\\\\\S+)))");

    // extract text containing url.
    const QRegularExpression re1("(?<url>^.*((ftp|https?)://\\S+)|(~?/\\S+).*$)");  // ftp http https ~/ /
    const QRegularExpression re2("(?<url>^.*(\\\\\\\\[^\\\\\\s]\\S+).*$)");         // windows unc.
    const QRegularExpression re3("(?<url>^.*([a-zA-Z]:\\\\[^\\s]*).*$)");           // windows local drive.

    QRegularExpression regs[] = { re1, re2, re3 };

    for(auto it = lines.begin(); it != lines.end(); it++) {

        QString line = *it;
        line = line.trimmed();

        if (line.isEmpty()) continue;
//qDebug() << line;

        for (const QRegularExpression& re : regs) {

            QRegularExpressionMatchIterator mi = re.globalMatch(line);

            while (mi.hasNext()) {

                QRegularExpressionMatch match = mi.next();
                QString url = match.captured("url");

                this->ui->listWidget->addItem(url);
                //
                writeLog(url);

                // scroll to last
                int lastRow = this->ui->listWidget->count() - 1;
                QListWidgetItem* lastItem = this->ui->listWidget->item(lastRow);
                this->ui->listWidget->scrollToItem(lastItem);
            }
        }
    }

}

#include <QStandardPaths>
#include <QDir>
#include <QTextStream>
#include <QDateTime>

void MainWindow::writeLog(const QString& log)
{
    QString path = QStandardPaths::writableLocation( QStandardPaths::DocumentsLocation );

    path = path + QDir::separator() + "UrlCollector.txt";

    QFile file(path);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
        return;
    }

    QDateTime now = QDateTime::currentDateTime();
    QString dt = now.toString("yyyyMMddhhmmss");

    QTextStream out(&file);
    out << dt << " " << log << Qt::endl;

    file.close();
}
