#include "../../../include/Form/PatientInput/Form_PatientInput.h"
#include "../../../include/Config/Config_ConfigLoader.h"
#include <QPushButton>
#include <QMessageBox>
#include <QScrollBar>
#include <QScrollArea>
#include <QVBoxLayout>

PatientInput::PatientInput(QWidget* parent):QDialog(parent){
    float basicHeight = 0.0;
    float basicWidth = 0.0;
    const std::map<unsigned int,patientInfoPair>* pattern = NULL;
    QWidget *backGround = new QWidget(this);
    QScrollArea* scrollArea = new QScrollArea(this);

    if(!parent){
        QMessageBox::critical(nullptr,"Error","The parent is not specialied!");
        exit(-1);
    }

    this->setGeometry(parent->geometry().x() + parent->geometry().width()*0.25,
                      parent->geometry().y() + parent->geometry().height()*0.2,
                      parent->geometry().width()*0.5,
                      parent->geometry().height()*0.6);

    this->setWindowFlags(this->windowFlags() | Qt::Window);
    this->setWindowModality(Qt::WindowModal);

    pattern = ConfigLoader::getInstance()->getThePatientInfoPatten();

    if(!pattern || pattern->size() <=0){
        QMessageBox::critical(nullptr,"Error","There are not patient info pattern specialed!");
        exit(-1);
    }

    this->clearPatternCompents();
    this->patternCompents = new std::map<unsigned int,patientInfoQtCompentsPair>();

    basicHeight = this->geometry().height()/10.0;
    basicWidth = this->geometry().width()/4.0;

    /*
    scrollbar->setGeometry(this->width()-19,0,19,this->height());
    scrollbar->setOrientation(Qt::Vertical);
    this->scrolbarVerticalMax = basicHeight*2*(pattern->size()+1);
    scrollbar->setMaximumHeight(this->scrolbarVerticalMax);
    QObject::connect(scrollbar,SIGNAL(valueChanged(int)),this,SLOT(scrollVertical(int)));
    */

    //scrollArea->setGeometry(0,0,this->geometry().width(),basicHeight*2*(pattern->size()+1));

    //backGround->setGeometry(0,0,this->geometry().width(),this->geometry().height());
    backGround->setGeometry(0,0,this->geometry().width(),basicHeight*2*(pattern->size()+1));

    for(std::map<unsigned int,patientInfoPair>::const_iterator it = pattern->begin();
                                                               it != pattern->end();
                                                               it++){
        QLabel *label = new QLabel(backGround);
        label->setText(it->second.first);
        label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        label->setGeometry(0.5*backGround->geometry().width() - basicWidth*1.1,
                           (it->first + 1)*2*basicHeight,
                           basicWidth,
                           basicHeight);

        QLineEdit *lineEdit = new QLineEdit(backGround);
        lineEdit->setGeometry(0.5*backGround->geometry().width(),
                              (it->first + 1)*2*basicHeight,
                              basicWidth,
                              basicHeight);

        this->patternCompents->insert(std::pair<unsigned int,patientInfoQtCompentsPair>(it->first,patientInfoQtCompentsPair(label,lineEdit)));
    }

    //backGround->hide();
    scrollArea->setWidget(backGround);
    //this->hide();
    //scrollArea->show();
    /*
    QObject::connect(this->uiForm->buttonBox->button(QDialogButtonBox::Ok),SIGNAL(pressed()),this,SLOT(acceptHandle()));
    QObject::connect(this->uiForm->buttonBox->button(QDialogButtonBox::Cancel),SIGNAL(pressed()),this,SLOT(rejectHandle()));
    */
}

PatientInput::~PatientInput(){

}

void PatientInput::closeEvent(QCloseEvent * event){
    QDialog::closeEvent(event);
    this->setResult(3);
}

void PatientInput::clearPatternCompents(){
    if(this->patternCompents){
        for(std::map<unsigned int,patientInfoQtCompentsPair>::iterator it = this->patternCompents->begin();
             it != this->patternCompents->end();
             it++){
            it->second.first->close();
            delete it->second.first;
            it->second.first = NULL;

            delete it->second.second;
            it->second.second = NULL;
        }

        std::map<unsigned int,patientInfoQtCompentsPair>().swap(*this->patternCompents);
        delete this->patternCompents;
        this->patternCompents = NULL;
    }
}

void PatientInput::clearInfos(){
    if(this->infos){
        std::map<unsigned int,QString>().swap(*this->infos);
        delete this->infos;
        this->infos = NULL;
    }
}

void PatientInput::clear(){
    this->clearPatternCompents();
    this->clearInfos();
}

void PatientInput::acceptHandle(){
    this->accept();
}

void PatientInput::rejectHandle(){
    this->reject();
}

void PatientInput::scrollVertical(int value){
    double p = value/this->scrolbarVerticalMax;
    this->scroll(0,-this->height()*p,QRect(0,0,this->width()-20,this->height()));
    this->repaint();
    //this->move(0,-this->height()*p);
}

void PatientInput::scrollHorizontal(int value){

}
