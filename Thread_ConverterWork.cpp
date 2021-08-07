#include "Thread_ConverterWork.h"
#include "ucConverter.h"
#include <QDebug>
#pragma execution_character_set("utf-8")

Thread_ConverterWork::Thread_ConverterWork(QObject* parent)
    : QThread(parent)
{

}
void Thread_ConverterWork::run()
{
    QFile delFile;
    delFile.setFileName(savePath + "/" + fileName + ".mp3");
    delFile.remove();
    QFile ucFile;
    QDir resultDir;
    resultDir.mkdir(savePath);
    ucFile.setFileName(filePath + "/" + fileName);
    if (!ucFile.open(QIODevice::ReadOnly)) { emit finish(return_row, false); return; }
    const UINT64 ucFileSize = ucFile.size();
    qDebug() << ucFileSize;
    QFile resultFile;
    resultFile.setFileName(savePath + "/" + fileName + ".mp3");
    if (!resultFile.open(QIODevice::Append)) { emit finish(return_row, false); return; }
    QByteArray byteBuffer;
    QByteArray writeBuffer;
    UINT64 nowPos = 0;
    UINT16 sendClock = 0;
    while (ucFile.atEnd() == false)
    {
        nowPos += 1;
        sendClock +=1;
        byteBuffer = ucFile.read(1);
        byteBuffer[0] = byteBuffer.at(0) ^ 163;
        writeBuffer.append(byteBuffer);
        //resultFile.write(byteBuffer);
        if (sendClock >= 50000)
        {
            resultFile.write(writeBuffer);
            writeBuffer.clear();
            emit reProgress(return_row, nowPos * 100 / ucFileSize);
            sendClock = 0;
        }
    }
    resultFile.write(writeBuffer);
    writeBuffer.clear();
    ucFile.close();
    resultFile.close();
    emit finish(return_row,true);
}