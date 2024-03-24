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
    backGround->setMinimumHeight(this->height());
    backGround->setMinimumWidth(this->width());
    backGround->setGeometry(0,0,this->geometry().width(),basicHeight*2*(pattern->size()+1.5));

    for(std::map<unsigned int,patientInfoPair>::const_iterator it = pattern->begin();
                                                               it != pattern->end();
                                                               it++){
        QLabel *label = new QLabel(backGround);
        label->setText(it->second.first);
        label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        label->setGeometry(0.45*backGround->geometry().width() - basicWidth*1.1,
                           (it->first*2 + 1)*basicHeight,
                           basicWidth,
                           basicHeight);

        QLineEdit *lineEdit = new QLineEdit(backGround);
        lineEdit->setGeometry(0.45*backGround->geometry().width(),
                              (it->first*2 + 1)*basicHeight,
                              basicWidth,
                              basicHeight);

        this->patternCompents->insert(std::pair<unsigned int,patientInfoQtCompentsPair>(it->first,patientInfoQtCompentsPair(label,lineEdit)));
    }

    /*Button*/
    this->buttonOK = new QPushButton(backGround);
    this->buttonOK->setText("OK");
    this->buttonOK->setGeometry(0.5*backGround->geometry().width() - basicWidth*1.1,
                                (pattern->size()*2 + 1)*basicHeight,
                                basicWidth,
                                basicHeight);

    this->buttonCancle = new QPushButton(backGround);
    this->buttonCancle->setText("Cancel");
    this->buttonCancle->setGeometry(0.5*backGround->geometry().width() + basicWidth*0.3,
                                   (pattern->size()*2 + 1)*basicHeight,
                                   basicWidth,
                                   basicHeight);

    QObject::connect(this->buttonOK,SIGNAL(pressed()),this,SLOT(acceptHandle()));
    QObject::connect(this->buttonCancle,SIGNAL(pressed()),this,SLOT(rejectHandle()));

    /*scrollArea*/
    scrollArea->setWidget(backGround);

}

PatientInput::~PatientInput(){
    this->clear();
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

    if(this->buttonOK){
        buttonOK->close();
        delete buttonOK;
    }

    if(this->buttonCancle){
        buttonCancle->close();
        delete buttonCancle;
    }
}

void PatientInput::acceptHandle(){
    this->clearInfos();
    if(this->patternCompents){
        this->infos = new std::map<unsigned int,QString>();
        for(std::map<unsigned int,patientInfoQtCompentsPair>::iterator it = this->patternCompents->begin();
                                                                       it != this->patternCompents->end();
                                                                       it++){
            if(this->infos->find(it->first) != this->infos->end()){
                QMessageBox::critical(nullptr,"Error",QString("Internal error: the patient info id %1 is repeated").arg(it->first));
                exit(-1);
            }
            this->infos->insert(std::pair<unsigned int,QString>(it->first,it->second.second->text()));
        }
    }

    this->accept();
}

void PatientInput::rejectHandle(){
    this->clearInfos();
    if(this->patternCompents){
        this->infos = new std::map<unsigned int,QString>();
        for(std::map<unsigned int,patientInfoQtCompentsPair>::iterator it = this->patternCompents->begin();
             it != this->patternCompents->end();
             it++){
            if(this->infos->find(it->first) != this->infos->end()){
                QMessageBox::critical(nullptr,"Error",QString("Internal error: the patient info id %1 is repeated").arg(it->first));
                exit(-1);
            }
            this->infos->insert(std::pair<unsigned int,QString>(it->first,""));
        }
    }

    this->reject();
}
