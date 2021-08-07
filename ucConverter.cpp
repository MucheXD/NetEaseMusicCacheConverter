#include "ucConverter.h"
#pragma execution_character_set("utf-8")

NMTC::NMTC(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    NMTC::reUi();
}
void NMTC::reUi()
{
    ui.table->setColumnWidth(0,0);
    ui.table->setColumnWidth(1,260);
    ui.table->setColumnWidth(2,100);
    ui.table->setColumnWidth(3,100);
    
    ui.table->verticalHeader()->setDefaultSectionSize(20);
    ui.pathEdit->setText( QDir::homePath()+"/AppData/Local/Netease/CloudMusic/Cache/Cache");
    ui.savePathEdit->setText(QCoreApplication::applicationDirPath()+"/result");
    ui.ca_progressBar->setVisible(false);
    connect(ui.pb_loadPath,&QPushButton::clicked,this,&NMTC::enumUcFile);
    connect(ui.toolButton, &QToolButton::clicked, this, &NMTC::selectPath);
    connect(ui.openPath, &QToolButton::clicked, this, &NMTC::openPath);
    connect(ui.pb_convertAll, &QPushButton::clicked, this, &NMTC::startConvertAll);
}
QStringList NMTC::enumUcFile(void)
{
    NMTC::lockMenu(true);
    QStringList fileNames;
    QStringList nameFilters;
    nameFilters.append("*.uc");
    QDir dir;
    dir.setPath(ui.pathEdit->text());
    fileNames = dir.entryList(nameFilters, QDir::Files | QDir::Readable, QDir::Name);
    int rdpos = 0;
    ui.table->clearContents();
    ui.table->setRowCount(fileNames.count());
    QFile file;
    while (rdpos+1<=fileNames.count())
    {
        QTableWidgetItem* item_fileName = new QTableWidgetItem;
        QTableWidgetItem* item_fileSize = new QTableWidgetItem;
        QComboBox* item_operate = new QComboBox;
        item_operate->addItem("");
        item_operate->addItem("转换");
        item_operate->addItem("移除");
        item_operate->addItem("删除文件");
        item_operate->setParent(ui.table);
        file.setFileName(ui.pathEdit->text()+"/"+ fileNames[rdpos]);
        item_fileName->setText(fileNames[rdpos]);
        item_fileSize->setText(QString::number(file.size()/1024)+"KB");
        ui.table->setItem(rdpos, 1, item_fileName);
        ui.table->setItem(rdpos, 2, item_fileSize);
        ui.table->setCellWidget(rdpos, 3, item_operate);
        connect(item_operate, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), this, &NMTC::tableItemOperate);
        rdpos += 1;
    }
    NMTC::lockMenu(false);
    return fileNames;
}
void NMTC::tableItemOperate(int currentIndex)
{
    QComboBox* IWC_copy = dynamic_cast<QComboBox*>(this->sender());//通过地址复制信号发送者,IWC=Item_Weights_ComboBox(信号发送者名称)
    QModelIndex IWC_Index = ui.table->indexAt(IWC_copy->pos());//获取发送者控件位置
    if (currentIndex == 1)
    {
        
        if(!NMTC::CallWork(IWC_Index.row()))
            IWC_copy->setCurrentIndex(0);
        /*
        QTableWidgetItem* item = ui.table->item(IWC_Index.row(), 1);
        QFileInfo file(ui.savePathEdit->text() + "/" + item->text()+".mp3");
        if (file.exists() == true)
        {
            if (QMessageBox::warning(this, "文件已存在", "当前目录下已有该文件:\n" + item->text() + "\n如果继续,原文件将被删除\n要继续吗?", QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
            {
                IWC_copy->setCurrentIndex(0);
                return;
            }
        }
        Thread_ConverterWork* workThread = new Thread_ConverterWork(this);
        workThread->fileName = item->text();
        workThread->filePath = ui.pathEdit->text();
        workThread->savePath = ui.savePathEdit->text();
        workThread->return_row = IWC_Index.row();
        workThread->start();
        connect(workThread, &Thread_ConverterWork::reProgress, this, &NMTC::refreshProgress);
        connect(workThread, &Thread_ConverterWork::finish, this, &NMTC::finishWork);
        ui.table->removeCellWidget(IWC_Index.row(),3);
        ui.table->setItem(IWC_Index.row(), 3, new QTableWidgetItem(""));
        NMTC::lockMenu(true);
        */
    }
    if (currentIndex == 2)
    {
        if (NMTC::nowWorkingCount >= 0)
        {
            QMessageBox::information(this,"任务进行中","当前有进行中的任务,请等待其运行完毕",QMessageBox::Ok);
            return;
        }
        ui.table->removeRow(IWC_Index.row());
    }
    if (currentIndex == 3)
    {
        if (NMTC::nowWorkingCount >= 0)
        {
            QMessageBox::information(this, "任务进行中", "当前有进行中的任务,请等待其运行完毕", QMessageBox::Ok);
            return;
        }
        QFile delFile;
        QTableWidgetItem* item = ui.table->item(IWC_Index.row(), 1);
        delFile.setFileName(ui.pathEdit->text()+"/"+item->text());
        const int ret=QMessageBox::question(this, "您真的要删除该文件吗", "以下文件将从您的计算机硬盘中移除:\n" + delFile.fileName(),QMessageBox::Yes|QMessageBox::No);
        if (ret == QMessageBox::Yes)
        {
            if (delFile.remove()==false)
            {
                QMessageBox::critical(this, "文件删除失败", "发生未知错误,无法删除以下文件:\n" + delFile.fileName(), QMessageBox::Ok);
            }
            else
            {
                ui.table->removeRow(IWC_Index.row());
            }
        }
    }
    IWC_copy->setCurrentIndex(0);
}
void NMTC::refreshProgress(int row,int progress)
{
    ui.table->item(row, 3)->setText(QString::number(progress)+"%");
    if (NMTC::isRunningAllConverting == true)
    {
        ui.ca_progressBar->setValue(ui.ca_progressBar->value()+1);
    }
}
void NMTC::finishWork(int row, bool isSuccess)
{
    if (NMTC::isRunningAllConverting == true)
    {
        if (NMTC::ca_nowConverting >= ui.table->rowCount()-1)
        {
            NMTC::lockMenu(false);
            ui.table->setEnabled(true);
            ui.ca_progressBar->setVisible(false);
            isRunningAllConverting = false;
            QMessageBox::information(this, "全部转换已完成", "当前线程已经转换完毕\n如果你使用了多线程,则需要等待另一线程执行完毕", QMessageBox::Ok);
        }
        else 
        {
            NMTC::ca_nowConverting += 1;
            ui.ca_progressBar->setValue(NMTC::ca_nowConverting*100);
            NMTC::CallWork(NMTC::ca_nowConverting);
        }
    }

    if (NMTC::nowWorkingCount>=0)
    {
        NMTC::nowWorkingCount -= 1;
    }

    if (isSuccess == true)
    {
        QPushButton* item_open = new QPushButton;
        item_open->setText("打开文件");
        ui.table->setCellWidget(row,3,item_open);
        connect(item_open, &QPushButton::clicked, this, &NMTC::openFile);
    }
    if (isSuccess == false)
    {
        QMessageBox::critical(this,"转换失败","发生未知错误导致文件转换失败\n请检查是否有其他程序在使用该文件",QMessageBox::Ok);
        ui.table->item(row, 3)->setText("转换失败");
    }
    if (NMTC::isRunningAllConverting == false)
    {
        NMTC::lockMenu(false);
    }
}
void NMTC::openFile(void)
{
    QPushButton* IWC_copy = dynamic_cast<QPushButton*>(this->sender());
    QModelIndex IWC_Index = ui.table->indexAt(IWC_copy->pos());
    qDebug() << ui.savePathEdit->text() + "/" + ui.table->item(IWC_Index.row(), 1)->text() + ".mp3";
    system("\"" + ui.savePathEdit->text().toLatin1() + "/" + ui.table->item(IWC_Index.row(), 1)->text().toLatin1() + ".mp3\"");
}
void NMTC::selectPath()
{
    ui.pathEdit->setText( QFileDialog::getExistingDirectory(this, "请选择网易云缓存文件所在的目录(.uc)", "./", NULL));
}
void NMTC::openPath()
{
    ui.savePathEdit->setText(QFileDialog::getExistingDirectory(this, "请选择要保存到的目录", ui.savePathEdit->text(), NULL));
}
void NMTC::lockMenu(bool lock)
{
    if (lock == true)
    {
        ui.pb_loadPath->setEnabled(false);
        ui.pathEdit->setEnabled(false);
        ui.savePathEdit->setEnabled(false);
        ui.toolButton->setEnabled(false);
        ui.openPath->setEnabled(false);
    }
    if (lock == false)
    {
        ui.pb_loadPath->setEnabled(true);
        ui.pathEdit->setEnabled(true);
        ui.savePathEdit->setEnabled(true);
        ui.toolButton->setEnabled(true);
        ui.openPath->setEnabled(true);
    }
}
bool NMTC::CallWork(int row)
{
    if (NMTC::nowWorkingCount >= 5)
    {
        QMessageBox::warning(this,"当前进行中任务过多","当前进行中任务过多,请等待其执行完毕",QMessageBox::Ok);
        return false;
    }
    QTableWidgetItem* item = ui.table->item(row, 1);
    QFileInfo file(ui.savePathEdit->text() + "/" + item->text() + ".mp3");
    if (file.exists() == true && NMTC::isRunningAllConverting==false)
    {
        if (QMessageBox::warning(this, "文件已存在", "当前目录下已有该文件:\n" + item->text() + "\n如果继续,原文件将被删除\n要继续吗?", QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
        {
            return false;
        }
    }
    Thread_ConverterWork* workThread = new Thread_ConverterWork(this);
    workThread->fileName = item->text();
    workThread->filePath = ui.pathEdit->text();
    workThread->savePath = ui.savePathEdit->text();
    workThread->return_row = row;
    workThread->start();
    connect(workThread, &Thread_ConverterWork::reProgress, this, &NMTC::refreshProgress);
    connect(workThread, &Thread_ConverterWork::finish, this, &NMTC::finishWork);
    ui.table->removeCellWidget(row, 3);
    ui.table->setItem(row, 3, new QTableWidgetItem(""));
    NMTC::lockMenu(true);
    NMTC::nowWorkingCount += 1;
    return true;
}
void NMTC::startConvertAll(void)
{
    if (ui.table->rowCount() <= 2)
    {
        QMessageBox::question(this, "口亨", "欸两个都不到也想要自动化?\n我才不帮你呢XD\n自己点一下嘛...", QMessageBox::Accepted);
        return;
    }
    const int msgBoxRet1= QMessageBox::question(this, "您真的要开始全部转换吗", "在全部转换完成前,您将不可操作程序\n在全部转换任务进行时,请不要关闭本窗口", QMessageBox::Yes | QMessageBox::No);
    if (msgBoxRet1 == QMessageBox::No)
        return;
    QDir dir(ui.savePathEdit->text());
    if (dir.count() > 2)
    {
        const int msgBoxRet2 = QMessageBox::warning(this, "目标保存目录中已有文件", "检测到您选择的目标保存文件夹中存在文件\n如果继续,则其中的同名文件会被替换\n要继续吗?", QMessageBox::Yes | QMessageBox::No);
        if (msgBoxRet2 == QMessageBox::No)
            return;
    }

    isRunningAllConverting = true;
    ca_nowConverting = 0;
    NMTC::CallWork(ca_nowConverting);
    if (ui.convertAllMode->currentIndex() == 1)
    {
        ca_nowConverting = 1;
        NMTC::CallWork(ca_nowConverting);
    }
    NMTC::lockMenu(true);
    ui.table->setEnabled(false);
    ui.ca_progressBar->setValue(0);
    ui.ca_progressBar->setMaximum(ui.table->rowCount()*100);
    ui.ca_progressBar->setVisible(true);
}