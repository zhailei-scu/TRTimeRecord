#include "../../../include/Form/TRTimeOperator/Form_TRTimeOperator_QueryForNextPatient.h"
#include <QPushButton>

/*************/
QueryNextPatientDialog::QueryNextPatientDialog(TRTimeOperator_Interface* parent):QDialog(parent){
    this->resize(parent->width()/4,parent->height()/5);
    this->setWindowTitle("Record inserted successful!");

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


void QueryNextPatientDialog::closeEvent(QCloseEvent * event){
    QDialog::closeEvent(event);
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
