#include "../../../include/Form/PatientInput/Form_PatientInput.h"
#include "ui_PatientInput.h"
#include <QPushButton>

PatientInput::PatientInput(QWidget* parent):QDialog(parent),uiForm(new Ui::PatientInput){
    uiForm->setupUi(this);

    QObject::connect(this->uiForm->buttonBox->button(QDialogButtonBox::Ok),SIGNAL(pressed()),this,SLOT(acceptHandle()));
    QObject::connect(this->uiForm->buttonBox->button(QDialogButtonBox::Cancel),SIGNAL(pressed()),this,SLOT(rejectHandle()));
}

PatientInput::~PatientInput(){

}

void PatientInput::closeEvent(QCloseEvent * event){
    QDialog::closeEvent(event);
    this->setResult(3);
}

void PatientInput::acceptHandle(){
    this->accept();
}

void PatientInput::rejectHandle(){
    this->reject();
}
