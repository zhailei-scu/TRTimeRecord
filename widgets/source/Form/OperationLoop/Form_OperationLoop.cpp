#include "../../../include/Form/OperationLoop/Form_OperationLoop.h"
#include "../../../include/Global/Communication/Global_Communication_Record.h"
#include "../../../include/Storage/DAO/Storage_DAO.h"
#include "../../../include/Storage/CSV/Storage_CSV_Writer.h"
#include "ui_OperationLoop.h"
#include <QMessageBox>
#include <sstream>
#include <QGraphicsDropShadowEffect>

OperationLoop::OperationLoop(QWidget* parent,unsigned int buttonID):QDialog(parent),uiForm(new Ui::OperationLoop){
    std::stringstream ss;
    QString tableName = "Date_";

    uiForm->setupUi(this);

    this->setWindowFlag(Qt::FramelessWindowHint);

    this->setStyleSheet(QString("QDialog{border:2px solid;"
                                        "border-color:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(255, 255, 0, 255), stop:1 rgba(255, 255, 255, 255));}"));

    this->buttonID = buttonID;

    std::map<unsigned int,OneOperationPattern>::const_iterator it = ConfigLoader::getInstance()->getTheOperationPatten()->find(buttonID);

    if(ConfigLoader::getInstance()->getTheOperationPatten()->end() == it){
        QMessageBox::critical(nullptr,"Error",QString("The button id %1 cannot find corresponded operation pattern").arg(buttonID));
        exit(-1);
    }

    this->onePattern = it->second;

    if(0 == this->buttonID){
        tableName.append(QDate::currentDate().toString("yyyy_MM_dd"));
        Record::getInstance()->lastTableName = tableName;
    }

    this->doneLoopIndex = 0;
    ss.str("");
    ss.clear();
    ss<<this->doneLoopIndex;
    this->uiForm->labelInner_CurrentIdex->setText(ss.str().c_str());

    ss.str("");
    ss.clear();
    if(this->onePattern.repeatTime > 0){
        ss<<this->onePattern.repeatTime;
    }else{
        ss<<"Unknown";
    }
    this->uiForm->labelInner_totalNum->setText(ss.str().c_str());

    if(this->onePattern.hintInfos.size() > this->doneLoopIndex){
        this->uiForm->labelCustom->setText(this->onePattern.hintInfos.at(this->doneLoopIndex));
    }

    if(this->onePattern.repeatTime > 0 && (signed int)this->doneLoopIndex >= this->onePattern.repeatTime){
        this->uiForm->nextIndexButton->setEnabled(false);
        this->uiForm->pauseButton->setEnabled(false);
    }

    this->uiForm->pauseButton->setToolTip("Pause");

    QObject::connect(this->uiForm->nextIndexButton,SIGNAL(pressed()),this,SLOT(nextIndexHandle()));
    QObject::connect(this->uiForm->abortButton,SIGNAL(pressed()),this,SLOT(abortHandle()));
    QObject::connect(this->uiForm->pauseButton,SIGNAL(pressed()),this,SLOT(pauseHandle()));
}

OperationLoop::~OperationLoop(){
    this->buttonID = 0;
    this->doneLoopIndex = 0;
    this->onePattern.clear();
    this->textBeforePause = "";
}

void OperationLoop::timeRecord(RunStatu statu){
    QString time = QTime::currentTime().toString("hh:mm:ss");

    std::map<unsigned int,QString>().swap(Record::getInstance()->buttonTimeRecord);
    Record::getInstance()->buttonTimeRecord.clear();
    Record::getInstance()->buttonTimeRecord.insert(std::pair<unsigned int,QString>(this->buttonID*3 + statu,time));

    DAO::getInstance()->getTrInfoConnection()->updateTableName(Record::getInstance()->lastTableName,
                                                         *ConfigLoader::getInstance()->getThePatientInfoPatten(),
                                                         *ConfigLoader::getInstance()->getTheOperationPatten());

    DAO::getInstance()->getTrInfoConnection()->appendARow(Record::getInstance()->lastTableName,Record::getInstance()->patientInfoRecord,Record::getInstance()->buttonTimeRecord);
    CSVWriter::getInstance()->appendARecord(Record::getInstance()->lastTableName,Record::getInstance()->patientInfoRecord,Record::getInstance()->buttonTimeRecord);
}

void OperationLoop::nextIndexHandle(){
    this->timeRecord(RunStatu(Normal));

    std::stringstream ss;

    this->doneLoopIndex++;

    if(this->onePattern.repeatTime > 0 && (signed int)this->doneLoopIndex >= this->onePattern.repeatTime){
        this->uiForm->nextIndexButton->setEnabled(false);
        this->uiForm->pauseButton->setEnabled(false);
        this->uiForm->label_Active->setText("Stop");
        this->uiForm->label_Active->setStyleSheet("background-color:red;color:white;");
    }

    ss.str("");
    ss.clear();
    ss<<this->doneLoopIndex;
    this->uiForm->labelInner_CurrentIdex->setText(ss.str().c_str());

    ss.str("");
    ss.clear();
    if(this->onePattern.repeatTime > 0){
        ss<<this->onePattern.repeatTime;
    }else{
        ss<<"Unknown";
    }
    this->uiForm->labelInner_totalNum->setText(ss.str().c_str());

    if(this->onePattern.hintInfos.size() > this->doneLoopIndex){
        this->uiForm->labelCustom->setText(this->onePattern.hintInfos.at(this->doneLoopIndex));
    }
}

void OperationLoop::abortHandle(){
    this->close();
}

void OperationLoop::pauseHandle(){
    if("Pause" == this->uiForm->pauseButton->toolTip()){
        this->timeRecord(RunStatu(Pause));
        this->uiForm->pauseButton->setIcon(QIcon(":/img/Continue.svg"));
        this->uiForm->pauseButton->setText("Continue");
        this->uiForm->nextIndexButton->setEnabled(false);
        this->uiForm->abortButton->setEnabled(false);
        this->textBeforePause = this->uiForm->labelCustom->text();
        this->uiForm->labelCustom->setText("Paused! Press \"Continue\" button to continue");
        this->uiForm->pauseButton->setToolTip("Continue");
    }else{
        this->timeRecord(RunStatu(Continue));
        this->uiForm->pauseButton->setIcon(QIcon(":/img/Pause.svg"));
        this->uiForm->pauseButton->setText("Pause");
        this->uiForm->nextIndexButton->setEnabled(true);
        this->uiForm->abortButton->setEnabled(true);
        this->uiForm->labelCustom->setText(this->textBeforePause);
        this->uiForm->pauseButton->setToolTip("Pause");
    }
}
