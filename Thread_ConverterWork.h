#pragma once
#include<QThread>
class Thread_ConverterWork :
    public QThread
{
    Q_OBJECT
public:
    QString fileName;
    QString filePath;
    QString savePath;
    int return_row;

    Thread_ConverterWork(QObject* parent);
    void test(void);
    void run();
signals:
    void reProgress(int return_row, int nowProgress);
    void finish(int row,bool isSuccess);
};

