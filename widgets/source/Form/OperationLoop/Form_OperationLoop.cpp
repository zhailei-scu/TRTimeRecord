#include "../../../include/Form/OperationLoop/Form_OperationLoop.h"
#include "ui_OperationLoop.h"
#include <QMessageBox>
#include <sstream>
#include <QGraphicsDropShadowEffect>

OperationLoop::OperationLoop(QWidget* parent,unsigned int buttonID):QDialog(parent),uiForm(new Ui::OperationLoop){
    std::stringstream ss;

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
    }

    QObject::connect(this->uiForm->nextIndexButton,SIGNAL(pressed()),this,SLOT(nextIndexHandle()));
    QObject::connect(this->uiForm->abortButton,SIGNAL(pressed()),this,SLOT(abortHandle()));
}

OperationLoop::~OperationLoop(){
    this->buttonID = 0;
    this->doneLoopIndex = 0;
    this->onePattern.clear();
}

bool OperationLoop::timeRecord(unsigned int buttonID){
    bool result = false;
    QString time = QTime::currentTime().toString("hh:mm:ss");
    QString tableName = "Date_";

    if(this->buttonTimeRecord.find(buttonID) == this->buttonTimeRecord.end()){
        this->buttonTimeRecord.insert(std::pair<unsigned int,QString>(buttonID,time));
    }else{
        QMessageBox::critical(nullptr, "Error", QString("Button Index %1 is repeated").arg(buttonID));
        exit(-1);
    }

    if((unsigned int)(buttonID+1) == ConfigLoader::getInstance()->getTheOperationPatten()->size()){
        tableName.append(QDate::currentDate().toString("yyyy_MM_dd"));

        DAO::getInstance()->updateTableName(tableName,
                                            *ConfigLoader::getInstance()->getThePatientInfoPatten(),
                                            *ConfigLoader::getInstance()->getTheOperationPatten());

        this->lastTableName = tableName;

        DAO::getInstance()->appendARow(tableName,patientInfoRecord,buttonTimeRecord);
        CSVWriter::getInstance()->appendARecord(tableName,patientInfoRecord,buttonTimeRecord);

        std::map<unsigned int,QString>().swap(this->patientInfoRecord);
        this->patientInfoRecord.clear();

        std::map<unsigned int,QString>().swap(this->buttonTimeRecord);
        this->buttonTimeRecord.clear();

        result = true;

        this->menuBar->getMenu("Setting")->actions().at(2)->setEnabled(true);
    }

    return result;
}

void OperationLoop::nextIndexHandle(){
    std::stringstream ss;

    this->doneLoopIndex++;

    if(this->onePattern.repeatTime > 0 && (signed int)this->doneLoopIndex >= this->onePattern.repeatTime){
        this->uiForm->nextIndexButton->setEnabled(false);
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
