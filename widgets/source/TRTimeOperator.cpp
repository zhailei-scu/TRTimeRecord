#include "../include/TRTimeOperator.h"
#include "../ui_TRTimeOperator.h"
#include "../include/DAO.h"
#include "../include/ConfigLoader.h"
#include "../include/Util.h"
#include <QMessageBox>
#include <QErrorMessage>
#include <QString>
#include <algorithm>
#include <QDateTime>
#include <QDate>
#include <QHBoxLayout>

TRTimeOperator::TRTimeOperator(QWidget* parent):QWidget(parent),uiForm(new Ui::TRTimeOperator){
    this->clear();
    uiForm->setupUi(this);

    this->uiConstruct();
}

TRTimeOperator::~TRTimeOperator(){
    this->clear();
}

void TRTimeOperator::uiConstruct(){
    new QHBoxLayout(this);

    this->menuBarConstruct();
    this->buttonConstruct();
}

void TRTimeOperator::uiDeconstruct(){
    if(this->menuBar){
        delete this->menuBar;
        this->menuBar = NULL;
    }

    if(this->buttonGroup){
        delete this->buttonGroup;
        this->buttonGroup = NULL;
    }
}

void TRTimeOperator::menuBarConstruct(){
    this->menuBar = new QMenuBar(this);

    this->menuBar->addMenu("Setting");
    this->menuBar->addSeparator();

    this->menuBar->addMenu("Data");
    this->menuBar->addSeparator();

    this->menuBar->addMenu("Help?");
    this->menuBar->addSeparator();

    this->layout()->setMenuBar(this->menuBar);
    this->menuBar->show();
}

void TRTimeOperator::buttonConstruct(){
    /*Registe button to buttonGroup for clicked id get and regist button to map*/
    /*To do: we need to config button based on configure file*/
    this->buttonGroup = new QButtonGroup(this);
    this->buttonsMap = new std::map<unsigned int,QAbstractButton*>();

    const auto begin = ConfigLoader::getInstance()->getTheOperatorPatten()->begin();
    const auto end = ConfigLoader::getInstance()->getTheOperatorPatten()->end();

    auto it = std::find_if(begin,end,map_value_finder<unsigned int,QString>("PatientComeIn"));
    if(it == end){
        QMessageBox::information(nullptr, "Error", QString("Button patten %1 is not found").arg("PatentComeIn"));
        exit(-1);
    }
    this->buttonGroup->addButton(this->uiForm->pushButton_PatentComeIn,it->first);
    this->buttonsMap->insert(std::pair<unsigned int,QAbstractButton*>(it->first,this->uiForm->pushButton_PatentComeIn));


    it = std::find_if(begin,end,map_value_finder<unsigned int,QString>("PatentImaging"));
    if(it == end){
        QMessageBox::information(nullptr, "Error", QString("Button patten %1 is not found").arg("PatentImaging"));
        exit(-1);
    }
    this->buttonGroup->addButton(this->uiForm->pushButton_PatentImaging,it->first);
    this->buttonsMap->insert(std::pair<unsigned int,QAbstractButton*>(it->first,this->uiForm->pushButton_PatentImaging));

    it = std::find_if(begin,end,map_value_finder<unsigned int,QString>("Theraphy"));
    if(it == end){
        QMessageBox::information(nullptr, "Error", QString("Button patten %1 is not found").arg("Theraphy"));
        exit(-1);
    }
    this->buttonGroup->addButton(this->uiForm->pushButton_Theraphy,it->first);
    this->buttonsMap->insert(std::pair<unsigned int,QAbstractButton*>(it->first,this->uiForm->pushButton_Theraphy));

    it = std::find_if(begin,end,map_value_finder<unsigned int,QString>("LeavingRoom"));
    if(it == end){
        QMessageBox::information(nullptr, "Error", QString("Button patten %1 is not found").arg("LeavingRoom"));
        exit(-1);
    }
    this->buttonGroup->addButton(this->uiForm->pushButton_LeavingRoom,it->first);
    this->buttonsMap->insert(std::pair<unsigned int,QAbstractButton*>(it->first,this->uiForm->pushButton_LeavingRoom));

    this->buttonGroup->setExclusive(true);

    this->changeButtonStatus(0);

    QObject::connect(this->buttonGroup,SIGNAL(idClicked(int)),this,SLOT(HandleSignal(int)));
}

void TRTimeOperator::clear(){
    this->uiDeconstruct();

    if(this->buttonsMap){
        std::map<unsigned int,QAbstractButton*>().swap(*this->buttonsMap);
        this->buttonsMap->clear();
        delete this->buttonsMap;
        this->buttonsMap = NULL;
    }

    std::map<unsigned int,QString>().swap(this->patientInfoRecord);
    this->patientInfoRecord.clear();

    std::map<unsigned int,QString>().swap(this->buttonTimeRecord);
    this->buttonTimeRecord.clear();
}

void TRTimeOperator::HandleSignal(int ID){
    //this->close();
    this->timeRecord(ID);
    this->changeButtonStatus(ID+1);
}

void TRTimeOperator::timeRecord(unsigned int buttonID){
    QString time = QTime::currentTime().toString("hh:mm:ss");
    QString tableName = "Date_";

    tableName.append(QDate::currentDate().toString("yyyy_MM_dd"));

    if(this->buttonTimeRecord.find(buttonID) == this->buttonTimeRecord.end()){
        this->buttonTimeRecord.insert(std::pair<unsigned int,QString>(buttonID,time));
    }else{
        QMessageBox::information(nullptr, "Error", QString("Button Index %1 is repeated").arg(buttonID));
        exit(-1);
    }

    if((unsigned int)(buttonID+1) == ConfigLoader::getInstance()->getTheOperatorPatten()->size()){
        DAO::getInstance()->appendARow(tableName,patientInfoRecord,buttonTimeRecord);

        std::map<unsigned int,QString>().swap(this->patientInfoRecord);
        this->patientInfoRecord.clear();

        std::map<unsigned int,QString>().swap(this->buttonTimeRecord);
        this->buttonTimeRecord.clear();

        QMessageBox::information(nullptr, "Information", "Record successful!");
    }
    /*
    int rowCount = model->rowCount();
    model->insertRow(rowCount);
    model->setData(model->index(0,0), QTime::currentTime().toString("hh:mm:ss"));
    model->submitAll();
    qDebug()<<tableName;
    QString time = QTime::currentTime().toString("hh:mm:ss");
    */
}

void TRTimeOperator::changeButtonStatus(unsigned int buttonID){
    if(0 >= this->buttonsMap->size()){
        QMessageBox::information(nullptr, "Error", "None button registerd!");
        return;
    }

    if(buttonID < this->buttonsMap->size()){
        std::map<unsigned int,QAbstractButton*>::iterator it = this->buttonsMap->begin();

        for(;it != this->buttonsMap->end(); it++){
            if(it->first != (unsigned int)buttonID){
                it->second->setEnabled(false);
                it->second->setStyleSheet("background-color:grey");
            }else{
                it->second->setEnabled(true);
                it->second->setStyleSheet("background-color:green");
            }
        }
    }else{
        if((unsigned int)buttonID == this->buttonsMap->size()){
            this->buttonsMap->at(buttonID-1)->setEnabled(false);
            this->buttonsMap->at(buttonID-1)->setStyleSheet("background-color:grey");
        }else{
            QMessageBox::information(nullptr, "Error", "Button ID exceed!");
            return;
        }
    }

}
