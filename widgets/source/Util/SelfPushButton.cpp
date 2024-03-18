#include "../../include/Util/SelfPushButton.h"
#include <QMessageBox>

SelfPushButton::SelfPushButton(QWidget *parent):QPushButton(parent){

}

SelfPushButton::~SelfPushButton(){

}

void SelfPushButton::mousePressEvent(QMouseEvent *e){
    QPushButton::mousePressEvent(e);
    QMessageBox::information(nullptr,"info","123");
    emit this->sign_handlePressEvent(this);
}
