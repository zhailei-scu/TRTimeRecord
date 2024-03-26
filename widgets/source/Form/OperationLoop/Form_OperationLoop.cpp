#include "../../../include/Form/OperationLoop/Form_OperationLoop.h"
#include "ui_OperationLoop.h"
#include <QMessageBox>
#include <sstream>

OperationLoop::OperationLoop(QWidget* parent,unsigned int buttonID):QDialog(parent),uiForm(new Ui::OperationLoop){
    std::stringstream ss;

    uiForm->setupUi(this);

    this->setWindowFlag(Qt::FramelessWindowHint);

    this->buttonID = buttonID;

    std::map<unsigned int,OneOperationPattern>::const_iterator it = ConfigLoader::getInstance()->getTheOperationPatten()->find(buttonID);

    if(ConfigLoader::getInstance()->getTheOperationPatten()->end() == it){
        QMessageBox::critical(nullptr,"Error",QString("The button id %1 cannot find corresponded operation pattern").arg(buttonID));
        exit(-1);
    }

    this->onePattern = it->second;

    this->currentLoopIndex = 1;
    ss.str("");
    ss.clear();
    ss<<this->currentLoopIndex<<"-th";
    this->uiForm->labelInner_CurrentIdex->setText(ss.str().c_str());

    ss.str("");
    ss.clear();
    if(this->onePattern.repeatTime > 0){
        ss<<this->onePattern.repeatTime;
    }else{
        ss<<"Unknown";
    }
    this->uiForm->labelInner_totalNum->setText(ss.str().c_str());

    if(this->onePattern.hintInfos.size() >= this->currentLoopIndex){
        this->uiForm->labelCustom->setText(this->onePattern.hintInfos.at(this->currentLoopIndex - 1));
    }

    QObject::connect(this->uiForm->nextIndexButton,SIGNAL(pressed()),this,SLOT(nextIndexHandle()));
    QObject::connect(this->uiForm->abortButton,SIGNAL(pressed()),this,SLOT(abortHandle()));
}

OperationLoop::~OperationLoop(){
    this->buttonID = 0;
    this->currentLoopIndex = 1;
    this->onePattern.clear();
}

void OperationLoop::nextIndexHandle(){
    std::stringstream ss;

    this->currentLoopIndex++;

    ss.str("");
    ss.clear();
    ss<<this->currentLoopIndex<<"-th";
    this->uiForm->labelInner_CurrentIdex->setText(ss.str().c_str());

    ss.str("");
    ss.clear();
    if(this->onePattern.repeatTime > 0){
        ss<<this->onePattern.repeatTime;
    }else{
        ss<<"Unknown";
    }
    this->uiForm->labelInner_totalNum->setText(ss.str().c_str());

    if(this->onePattern.hintInfos.size() >= this->currentLoopIndex){
        this->uiForm->labelCustom->setText(this->onePattern.hintInfos.at(this->currentLoopIndex - 1));
    }
}

void OperationLoop::abortHandle(){
    this->close();
}
