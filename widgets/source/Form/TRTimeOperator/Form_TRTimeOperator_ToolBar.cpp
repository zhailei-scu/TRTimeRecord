#include "../../../include/Form/TRTimeOperator/Form_TRTimeOperator_ToolBar.h"
#include <QHBoxLayout>
#include <QToolButton>
#include <QMenu>

OperatorToolBar::OperatorToolBar(TRTimeOperator_Interface* parent):QToolBar(parent){

}

OperatorToolBar::~OperatorToolBar(){

}

void OperatorToolBar::uiConstruct(const QRect & geometry){
    this->toolsMap = new std::map<QString,QToolButton*>();

    QToolButton *buttonSetting = new QToolButton();
    buttonSetting->setIcon(QIcon(":/img/setting.svg"));
    buttonSetting->setText("Setting");
    this->addWidget(buttonSetting);
    buttonSetting->addAction("Storage");
    buttonSetting->addAction("PatientInfo");
    buttonSetting->addAction("Pipline");
    buttonSetting->addAction("NetWork");
    this->toolsMap->insert(std::pair<QString,QToolButton*>("Setting",buttonSetting));
    buttonSetting->setPopupMode(QToolButton::InstantPopup);

    QToolButton *dataView = new QToolButton();
    dataView->setIcon(QIcon(":/img/dataBase.svg"));
    dataView->setText("DataView");
    this->addWidget(dataView);
    dataView->addAction("CSV Viewer");
    dataView->addAction("DataBase Viewer");
    this->toolsMap->insert(std::pair<QString,QToolButton*>("DataView",dataView));
    dataView->setPopupMode(QToolButton::InstantPopup);

    QToolButton *sync = new QToolButton();
    sync->setIcon(QIcon(":/img/sync.svg"));
    sync->setText("sync");
    this->addWidget(sync);
    this->toolsMap->insert(std::pair<QString,QToolButton*>("sync",sync));

    QToolButton *Port = new QToolButton();
    Port->setIcon(QIcon(":/img/Port.svg"));
    Port->setText("Port");
    this->addWidget(Port);
    Port->setCheckable(false);
    this->toolsMap->insert(std::pair<QString,QToolButton*>("Port",Port));
    Port->setPopupMode(QToolButton::InstantPopup);

    //this->layout()->addWidget(this->toolBar);

    //this->layout()->setAlignment(Qt::AlignTop);
    this->setGeometry(geometry);

    this->show();
}

void OperatorToolBar::uiDeConstruct(){
    if(this->toolsMap){
        for(std::map<QString,QToolButton*>::iterator it = this->toolsMap->begin();
                                                     it != this->toolsMap->end();
                                                     it++){
            it->second->close();
        }
        std::map<QString,QToolButton*>().swap(*this->toolsMap);
        this->toolsMap->clear();
        delete this->toolsMap;
        this->toolsMap = NULL;
    }
}
