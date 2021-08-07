#pragma once

#include <QtWidgets/QWidget>
#include <QFile>
#include<QFileDialog>
#include <QDir>
#include <QDebug>
#include <Windows.h>
#include <thread>
#include <QMessageBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QProgressbar>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include "ui_mainWindow.h"
#include "Thread_ConverterWork.h"

class NMTC : public QWidget
{
    Q_OBJECT

public:

    UINT8 nowWorkingCount = 0;
    UINT16 ca_nowConverting = 0;
    bool isRunningAllConverting = false;

    NMTC(QWidget *parent = Q_NULLPTR);
    void reUi(void);
    QStringList enumUcFile(void);
    void tableItemOperate(int);
    void refreshProgress(int ,int);
    void finishWork(int,bool);
    void openFile(void);
    void selectPath(void);
    void openPath(void);
    void lockMenu(bool);
    bool CallWork(int);
    void startConvertAll(void);

    Ui::NMTCClass ui;

private:

};
