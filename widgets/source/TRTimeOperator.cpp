#include "../include/TRTimeOperator.h"
#include "../ui_TRTimeOperator.h"
#include <QMessageBox>
#include <QString>
#include <sstream>
TRTimeOperator::TRTimeOperator(QWidget* parent):QWidget(parent),uiForm(new Ui::TRTimeOperator){
    this->clear();
    uiForm->setupUi(this);
    this->buttonGroup = new QButtonGroup(this);
    this->buttonGroup->addButton(this->uiForm->pushButton_PatentComeIn,0);
    this->buttonGroup->addButton(this->uiForm->pushButton_PatentImaging,1);
    this->buttonGroup->addButton(this->uiForm->pushButton_Theraphy,2);
    this->buttonGroup->addButton(this->uiForm->pushButton_LeavingRoom,3);
    this->buttonGroup->setExclusive(true);

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
}

void TRTimeOperator::HandleSignal(int ID){
    //this->close();
    std::stringstream ss;
    ss<< ID << " Button was clicked!";

    QMessageBox::information(nullptr, "Message", ss.str().c_str());
}
