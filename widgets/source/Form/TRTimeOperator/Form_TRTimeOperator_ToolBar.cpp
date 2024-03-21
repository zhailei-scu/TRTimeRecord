#include "../../../include/Form/TRTimeOperator/Form_TRTimeOperator_ToolBar.h"
#include <QHBoxLayout>
#include <QToolButton>

OperatorToolBar::OperatorToolBar(TRTimeOperator_Interface* parent):QToolBar(parent){

}

OperatorToolBar::~OperatorToolBar(){

}

void OperatorToolBar::uiConstruct(const QRect & geometry){

    QToolButton *buttonSetting = new QToolButton();
    buttonSetting->setIcon(QIcon(":/img/setting.svg"));
    buttonSetting->setText("Setting");
    this->addWidget(buttonSetting);

    QToolButton *dataView = new QToolButton();
    dataView->setIcon(QIcon(":/img/dataBase.svg"));
    dataView->setText("DataView");
    this->addWidget(dataView);

    //this->layout()->addWidget(this->toolBar);

    //this->layout()->setAlignment(Qt::AlignTop);
    this->setGeometry(geometry);

    this->show();
}

void OperatorToolBar::uiDeConstruct(){

}
