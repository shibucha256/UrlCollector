/*
 *  mainwindow.h: receive clipboard contents.
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void clipBoardChanged();

signals:
    void textFound(const QString& text);

public slots:
    void textArrived(const QString& text);

protected:
    void writeLog(const QString& log);

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
