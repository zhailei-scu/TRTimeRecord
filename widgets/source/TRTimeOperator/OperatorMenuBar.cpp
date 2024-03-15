#include "../../include/TRTimeOperator/OperatorMenuBar.h"
#include <QHBoxLayout>
#include <QMessageBox>

OperatorMenuBar::OperatorMenuBar(TRTimeOperator_Interface* parent):QMenuBar(parent){
    this->clear();
    this->menuMap = new std::map<QString,const QMenu &>();
}

OperatorMenuBar::~OperatorMenuBar(){
    this->clear();
}

void OperatorMenuBar::uiConstruct(const QRect & geometry){

    QMenu *menu = this->addMenu("Setting");
    this->menuMap->insert(std::pair<QString,const QMenu &>("Setting",*menu));
    menu->addAction("Storage");
    menu->addAction("Pipline");

    //this->menuBar->addSeparator();

    menu = this->addMenu("DataView");
    this->menuMap->insert(std::pair<QString,const QMenu &>("DataView",*menu));
    QAction *action = new QAction("DataBase",this);
    QObject::connect(action,&QAction::triggered,this,&OperatorMenuBar::dataView);
    menu->addAction(action);

    //this->menuBar->addSeparator();

    this->addMenu("Help?");
    //this->menuBar->addSeparator();

    this->setGeometry(geometry);
    //this->layout()->setMenuBar(this->menuBar);

    new QHBoxLayout();

    this->show();
}

void OperatorMenuBar::uiDeConstruct(){

}

void OperatorMenuBar::clear(){
    if(this->menuMap){
        std::map<QString,const QMenu &>().swap(*this->menuMap);
        this->menuMap->clear();
        delete this->menuMap;
        this->menuMap = NULL;
    }
}

/*action slot*/
void OperatorMenuBar::dataView(){
    QMessageBox::information(nullptr,"Info","Triggered");
}
