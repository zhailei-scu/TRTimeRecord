#include "../../../include/Common/Ui/Common_Ui_SelfPushButton.h"
#include <QMessageBox>

SelfPushButton::SelfPushButton(QWidget *parent):QPushButton(parent){

}

SelfPushButton::~SelfPushButton(){

}

void SelfPushButton::mousePressEvent(QMouseEvent *e){
    QPushButton::mousePressEvent(e);
    emit this->sign_handlePressEvent(this);
}
