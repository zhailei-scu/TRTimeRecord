#include "../../include/TRTimeOperator/TRTimeOperator.h"
#include "../ui_TRTimeOperator.h"
#include "../../include/TRTimeOperator/QueryForNextPatient.h"
#include "../../include/DAO/DAO.h"
#include "../../include/Config/ConfigLoader.h"
#include "../../include/Util/Util.h"
#include <QMessageBox>
#include <QErrorMessage>
#include <QString>
#include <algorithm>
#include <QDateTime>
#include <QDate>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QToolButton>
#include <QTabBar>

TRTimeOperator::TRTimeOperator(TRTimeOperator_Interface* parent):TRTimeOperator_Interface(parent),uiForm(new Ui::TRTimeOperator){
    this->clear();
    uiForm->setupUi(this);
    this->uiConstruct();
}

TRTimeOperator::~TRTimeOperator(){
    this->clear();
}

void TRTimeOperator::uiConstruct(){
    new QHBoxLayout(this); //registe a Layout object for current window
    this->layout()->setGeometry(this->geometry());

    this->menuBar = new OperatorMenuBar(this);
    this->menuBar->uiConstruct(QRect(0,
                                     0,
                                     this->geometry().width(),
                                     this->geometry().height()/25));

    QObject::connect(*this->menuBar->getMenu("DataView")->actions().begin(),&QAction::triggered,this,&TRTimeOperator::dataView);

    this->toolBar = new OperatorToolBar(this);
    this->toolBar->uiConstruct(QRect(0,
                                     this->geometry().height()/25,
                                     this->geometry().width(),
                                     this->geometry().height()/25));

    this->uiForm->tabWidget->setGeometry(0,
                                         this->toolBar->geometry().bottom(),
                                         this->geometry().width(),
                                         this->geometry().height() - this->toolBar->geometry().bottom());

    QObject::connect(this->uiForm->tabWidget,SIGNAL(tabCloseRequested(int)),this,SLOT(removeTable(int)));

    this->uiForm->tabWidget->setTabsClosable(true);
    this->uiForm->tabWidget->tabBar()->setTabButton(0,QTabBar::RightSide,nullptr);

    this->buttonConstruct();
}

void TRTimeOperator::uiDeconstruct(){
    if(this->menuBar){
        this->menuBar->uiDeConstruct();
        delete this->menuBar;
        this->menuBar = NULL;
    }

    if(this->toolBar){
        this->toolBar->uiDeConstruct();
        delete this->toolBar;
        this->toolBar = NULL;
    }

    if(this->buttonGroup){
        delete this->buttonGroup;
        this->buttonGroup = NULL;
    }
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

    this->lastTableName = "";
}

void TRTimeOperator::HandleSignal(int ID){
    bool recorded = this->timeRecord(ID);
    this->changeButtonStatus(ID+1);

    if(recorded){
        this->queryForNextPatient();
    }
}

void TRTimeOperator::removeTable(int index){
    this->uiForm->tabWidget->removeTab(index);
}

void TRTimeOperator::dataView(){
    //QTableModel *table = new
    //this->uiForm->tabWidget->addTab();
}

bool TRTimeOperator::timeRecord(unsigned int buttonID){
    bool result = false;
    QString time = QTime::currentTime().toString("hh:mm:ss");
    QString tableName = "Date_";

    tableName.append(QDate::currentDate().toString("yyyy_MM_dd"));

    this->lastTableName = tableName;

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
    QueryNextPatientDialog *dialog = new QueryNextPatientDialog(this);
    /*
    QueryNextPatientDialog (this);                  //Please do not allocate the QDialog in stack when some compents within
                                                    //it are allocated in heap, because it would not be released automate
                                                    //, besides, the Qt::WA_DeleteOnClose flag is also not available(see the)
                                                    //(explain below).

    dialog.setAttribute(Qt::WA_DeleteOnClose);      //Please call this function careful, beacause
                                                    //if some compents within the object and allocated in heap
                                                    //would be released manually when deconstructor are called,
                                                    //but this flag would also release the compents, so
                                                    //the compents would be release two times, so the memory error would
                                                    //occur.
    */
    int code = dialog->exec();
    if(QDialog::Accepted == code){
        this->changeButtonStatus(0);
    }else if(QDialog::Rejected == code){
        bool OK = true;
        QString inputed;
        while(!(inputed = QInputDialog::getText(this,QString("Delete confirm"),QString("Please input 'hfimc'"),QLineEdit::Password,0,&OK)).isEmpty() && OK && (inputed != QString("hfimc"))){
            QMessageBox::information(nullptr,"Error",QString("Input: %1, you should input 'hficm'").arg(inputed));
        }
        if(OK){
            DAO::getInstance()->deleteLastRecord(this->lastTableName);
        }
        this->changeButtonStatus(0);
    }else{
        this->changeButtonStatus(0);
    }

    if(dialog){
        delete dialog;
        dialog = NULL;
    }
}