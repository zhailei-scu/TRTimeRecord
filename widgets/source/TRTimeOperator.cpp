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
    new QHBoxLayout(this); //registe a Layout object for current window

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
        QMessageBox::critical(nullptr, "Error", QString("Button patten %1 is not found").arg("PatentComeIn"));
        exit(-1);
    }
    this->buttonGroup->addButton(this->uiForm->pushButton_PatentComeIn,it->first);
    this->buttonsMap->insert(std::pair<unsigned int,QAbstractButton*>(it->first,this->uiForm->pushButton_PatentComeIn));


    it = std::find_if(begin,end,map_value_finder<unsigned int,QString>("PatentImaging"));
    if(it == end){
        QMessageBox::critical(nullptr, "Error", QString("Button patten %1 is not found").arg("PatentImaging"));
        exit(-1);
    }
    this->buttonGroup->addButton(this->uiForm->pushButton_PatentImaging,it->first);
    this->buttonsMap->insert(std::pair<unsigned int,QAbstractButton*>(it->first,this->uiForm->pushButton_PatentImaging));

    it = std::find_if(begin,end,map_value_finder<unsigned int,QString>("Theraphy"));
    if(it == end){
        QMessageBox::critical(nullptr, "Error", QString("Button patten %1 is not found").arg("Theraphy"));
        exit(-1);
    }
    this->buttonGroup->addButton(this->uiForm->pushButton_Theraphy,it->first);
    this->buttonsMap->insert(std::pair<unsigned int,QAbstractButton*>(it->first,this->uiForm->pushButton_Theraphy));

    it = std::find_if(begin,end,map_value_finder<unsigned int,QString>("LeavingRoom"));
    if(it == end){
        QMessageBox::critical(nullptr, "Error", QString("Button patten %1 is not found").arg("LeavingRoom"));
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
    bool recorded = this->timeRecord(ID);
    this->changeButtonStatus(ID+1);

    if(recorded){
        this->queryForNextPatient();
    }
}

bool TRTimeOperator::timeRecord(unsigned int buttonID){
    bool result = false;
    QString time = QTime::currentTime().toString("hh:mm:ss");
    QString tableName = "Date_";

    tableName.append(QDate::currentDate().toString("yyyy_MM_dd"));

    if(this->buttonTimeRecord.find(buttonID) == this->buttonTimeRecord.end()){
        this->buttonTimeRecord.insert(std::pair<unsigned int,QString>(buttonID,time));
    }else{
        QMessageBox::critical(nullptr, "Error", QString("Button Index %1 is repeated").arg(buttonID));
        exit(-1);
    }

    if((unsigned int)(buttonID+1) == ConfigLoader::getInstance()->getTheOperatorPatten()->size()){
        DAO::getInstance()->appendARow(tableName,patientInfoRecord,buttonTimeRecord);

        std::map<unsigned int,QString>().swap(this->patientInfoRecord);
        this->patientInfoRecord.clear();

        std::map<unsigned int,QString>().swap(this->buttonTimeRecord);
        this->buttonTimeRecord.clear();

        result = true;

        QMessageBox::information(nullptr, "Information", "Record successful!");
    }

    return result;
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
        QMessageBox::critical(nullptr, "Error", "None button registerd!");
        exit(-1);
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
            QMessageBox::critical(nullptr, "Error", "Button ID exceed!");
            exit(-1);
        }
    }

}

void TRTimeOperator::queryForNextPatient(){

    QueryNextPatientDialog dialog(this);
    int code = dialog.exec();
    qDebug()<<code;
    if(QDialog::Accepted == code){
        QMessageBox::information(nullptr,"Info","Next");
    }else if(QDialog::Rejected == code){
        QMessageBox::information(nullptr,"Info","DeleteRecord");
    }else{
        QMessageBox::information(nullptr,"Info","Noting to do");
    }
}

/*************/
QueryNextPatientDialog::QueryNextPatientDialog(TRTimeOperator* parent):QDialog(parent){
    //this->setAttribute(Qt::WA_DeleteOnClose);
    this->resize(parent->width()/4,parent->height()/5);
    this->setWindowTitle("Ready for next?");

    this->nextPatient = new QPushButton(this);
    this->nextPatient->setGeometry(QRect(0,
                                         0,
                                         this->geometry().width()/2,
                                         this->geometry().height()));
    this->nextPatient->setText("Next Patient");

    this->deleteRecord = new QPushButton(this);
    this->deleteRecord->setText("Delete record");
    this->deleteRecord->setGeometry(QRect(this->geometry().width()/2,
                                          0,
                                          this->geometry().width()/2,
                                          this->geometry().height()));

    QObject::connect(this->nextPatient,SIGNAL(pressed()),this,SLOT(nextPatientHandle()));
    QObject::connect(this->deleteRecord,SIGNAL(pressed()),this,SLOT(deleteRecordHandle()));
}

QueryNextPatientDialog::~QueryNextPatientDialog(){
    if(this->nextPatient){
        delete this->nextPatient;
        this->nextPatient = NULL;
    }

    if(this->deleteRecord){
        delete this->deleteRecord;
        this->deleteRecord = NULL;
    }
}


void QueryNextPatientDialog::closeEvent(QCloseEvent *){
    this->setResult(3);
}

void QueryNextPatientDialog::nextPatientHandle(){
    //this->setResult(QDialog::Accepted);
    //this->close();
    this->accept();
}

void QueryNextPatientDialog::deleteRecordHandle(){
    //this->setResult(QDialog::Rejected);
    //this->close();
    this->reject();
}

