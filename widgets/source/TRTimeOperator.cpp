#include "../include/TRTimeOperator.h"
#include "../ui_TRTimeOperator.h"
#include <QMessageBox>
#include <QErrorMessage>
#include <QString>
#include <sstream>
#include <QDateTime>
#include <QDate>

TRTimeOperator::TRTimeOperator(QWidget* parent):QWidget(parent),uiForm(new Ui::TRTimeOperator){
    this->clear();
    uiForm->setupUi(this);

    /*Registe button to buttonGroup for clicked id get and regist button to map*/
    this->buttonGroup = new QButtonGroup(this);
    this->buttonsMap = new std::map<int,QAbstractButton*>();

    this->buttonGroup->addButton(this->uiForm->pushButton_PatentComeIn,0);
    this->buttonsMap->insert(std::pair<int,QAbstractButton*>(0,this->uiForm->pushButton_PatentComeIn));

    this->buttonGroup->addButton(this->uiForm->pushButton_PatentImaging,1);
    this->buttonsMap->insert(std::pair<int,QAbstractButton*>(1,this->uiForm->pushButton_PatentImaging));

    this->buttonGroup->addButton(this->uiForm->pushButton_Theraphy,2);
    this->buttonsMap->insert(std::pair<int,QAbstractButton*>(2,this->uiForm->pushButton_Theraphy));

    this->buttonGroup->addButton(this->uiForm->pushButton_LeavingRoom,3);
    this->buttonsMap->insert(std::pair<int,QAbstractButton*>(3,this->uiForm->pushButton_LeavingRoom));

    this->buttonGroup->setExclusive(true);

    this->changeButtonStatus(0);

    QObject::connect(this->buttonGroup,SIGNAL(idClicked(int)),this,SLOT(HandleSignal(int)));
}

TRTimeOperator::~TRTimeOperator(){
    this->clear();
}

void TRTimeOperator::setupUi(TRTimeOperator*){

}

void TRTimeOperator::clear(){
    if(this->buttonGroup){
        delete this->buttonGroup;
        this->buttonGroup = NULL;
    }

    if(this->buttonsMap){
        std::map<int,QAbstractButton*>().swap(*this->buttonsMap);
        this->buttonsMap->clear();
        delete this->buttonsMap;
        this->buttonsMap = NULL;
    }
}

void TRTimeOperator::HandleSignal(int ID){
    //this->close();
    this->timeRecord(ID);
    this->changeButtonStatus(ID+1);

    std::stringstream ss;
    ss<< ID << " Button was clicked!";

    QMessageBox::information(nullptr, "Message", ss.str().c_str());
}


void TRTimeOperator::timeRecord(int buttonID){
    QString date = QDate::currentDate().toString("yyyy-MM-dd");
    qDebug()<<date;
    QString time = QTime::currentTime().toString("hh:mm:ss");
    qDebug()<<time;
}

void TRTimeOperator::changeButtonStatus(int buttonID){
    if(0 >= this->buttonsMap->size()){
        QMessageBox::information(nullptr, "Error", "None button registerd!");
        return;
    }

    if(uint(buttonID) < this->buttonsMap->size()){
        std::map<int,QAbstractButton*>::iterator it = this->buttonsMap->begin();

        for(;it != this->buttonsMap->end(); it++){
            if(it->first != buttonID){
                it->second->setEnabled(false);
                it->second->setStyleSheet("background-color:grey");
            }else{
                it->second->setEnabled(true);
                it->second->setStyleSheet("background-color:green");
            }
        }
    }else{
        if(uint(buttonID) == this->buttonsMap->size()){
            this->buttonsMap->at(buttonID-1)->setEnabled(false);
            this->buttonsMap->at(buttonID-1)->setStyleSheet("background-color:grey");
        }else{
            QMessageBox::information(nullptr, "Error", "Button ID exceed!");
            return;
        }
    }

}
