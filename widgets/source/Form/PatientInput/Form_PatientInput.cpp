#include "../../../include/Form/PatientInput/Form_PatientInput.h"
#include "../../../include/Global/Config/Global_Config_ConfigLoader.h"
#include "../../../include/Common/Util/Common_Util_Base.h"
#include "../../../include/Storage/DAO/Storage_DAO.h"
#include <QPushButton>
#include <QMessageBox>
#include <QScrollBar>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QComboBox>
#include <QCompleter>

PatientInput::PatientInput(QWidget* parent,std::map<unsigned int,std::pair<QString,QString>> *info,PatientInputMode model):QDialog(parent){
    float basicHeight = 0.0;
    float basicWidth = 0.0;
    QLabel *label = NULL;
    QWidget *editContent = NULL;
    QStringList list;
    std::map<unsigned int,std::pair<QString,QString>>::iterator it_find;

    this->currentMode = model;

    const std::map<unsigned int,OnePatientPattern>* pattern = NULL;
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

    if(PatientInputMode(Modify) == model){
        this->setWindowTitle("Please input patient information.");
    }else{
        this->setWindowTitle("Confim the patient information");
    }

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

    for(std::map<unsigned int,OnePatientPattern>::const_iterator it = pattern->begin();
                                                                 it != pattern->end();
                                                                 it++){
        label = new QLabel(backGround);
        label->setText(it->second.labelName);
        label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        label->setGeometry(0.45*backGround->geometry().width() - basicWidth*1.1,
                           (it->first*2 + 1)*basicHeight,
                           basicWidth,
                           basicHeight);

        if("true" != it->second.supportPreQuery){
            editContent = new QLineEdit(backGround);
        }else{
            editContent = new QComboBox(backGround);
            ((QComboBox*)editContent)->setStyleSheet("QComboBox::down-arrow{image:none;}");
            ((QComboBox*)editContent)->setEditable(true);
            QStringList().swap(list);
            list.clear();
            if(DAO::getInstance()->getPatientInfoConnection()->columnExisted_TR(it->second.infoName)){
                DAO::getInstance()->getPatientInfoConnection()->getAllValueByKey_Patient(it->second.infoName,list);
            }else{
                qDebug()<<"Not existed..."<<it->second.infoName;
            }
            QCompleter *finder = new QCompleter(list,editContent);
            ((QComboBox*)editContent)->setCompleter(finder);
            finder->setObjectName(it->second.infoName);

            //QObject::connect((QComboBox*)editContent,SIGNAL(textActivated(const QString &)),this,SLOT(fillInfo(const QString &)));
            QObject::connect(finder,SIGNAL(activated(const QString &)),this,SLOT(fillInfo(const QString &)));
        }

        editContent->setGeometry(0.45*backGround->geometry().width(),
                                 (it->first*2 + 1)*basicHeight,
                                 basicWidth,
                                 basicHeight);

        if(NULL != info && info->find(it->first) != info->end()){
            it_find = std::find_if(info->begin(),
                                   info->end(),
                                   map_value_finder_PairInValue<unsigned int,QString,QString>(it->second.infoName));

            if(info->end() != it_find){
                if("true" != it->second.supportPreQuery){
                    dynamic_cast<QLineEdit*>(editContent)->setText(it_find->second.second);
                }else{
                    dynamic_cast<QComboBox*>(editContent)->setCurrentText(it_find->second.second);
                }
            }
        }

        if(PatientInputMode(ViewAndModify) == model){
            editContent->setEnabled(false);
        }

        if(it->second.unRemoveable){
            editContent->setStyleSheet("background-color:yellow");
        }

        this->patternCompents->insert(std::pair<unsigned int,patientInfoQtCompentsPair>(it->first,patientInfoQtCompentsPair(label,editContent)));
    }

    /*Button*/
    this->buttonOK = new QPushButton(backGround);
    if(PatientInputMode(Modify) == model){
        this->buttonOK->setText("OK");
    }else{
        this->buttonOK->setText("NO Problem");
    }
    this->buttonOK->setGeometry(0.5*backGround->geometry().width() - basicWidth*1.1,
                                (pattern->size()*2 + 1)*basicHeight,
                                basicWidth,
                                basicHeight);

    this->buttonCancle = new QPushButton(backGround);

    if(PatientInputMode(Modify) == model){
        this->buttonCancle->setText("Cancel");
    }else{
        this->buttonCancle->setText("Modify");
    }

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
    this->clearInfos();
    if(this->patternCompents){
        this->infos = new std::map<unsigned int,QString>();
        for(std::map<unsigned int,patientInfoQtCompentsPair>::iterator it = this->patternCompents->begin();
             it != this->patternCompents->end();
             it++){
            this->infos->insert(std::pair<unsigned int,QString>(it->first,""));
        }
    }

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
    bool flag = true;
    QString content = "";
    this->clearInfos();
    const std::map<unsigned int,OnePatientPattern>* pattern = ConfigLoader::getInstance()->getThePatientInfoPatten();
    if(this->patternCompents){
        this->infos = new std::map<unsigned int,QString>();

        for(std::map<unsigned int,patientInfoQtCompentsPair>::iterator it = this->patternCompents->begin();
                                                                       it != this->patternCompents->end();
                                                                       it++){
            if("true" != pattern->at(it->first).supportPreQuery){
                content = dynamic_cast<QLineEdit*>(it->second.second)->text();
            }else{
                content = dynamic_cast<QComboBox*>(it->second.second)->currentText();
            }

            if("true" == pattern->at(it->first).necessary && "" == content){
                QMessageBox::critical(nullptr,"Error",QString("The item %1 cannot be empty").arg(pattern->at(it->first).labelName));
                flag = false;
                break;
            }
            this->infos->insert(std::pair<unsigned int,QString>(it->first,content));
        }

    }

    if(flag){
        this->accept();
    }
}

void PatientInput::rejectHandle(){
    this->clearInfos();

    if(PatientInputMode(Modify) == this->currentMode){
        this->reject();
    }else{
        this->currentMode = PatientInputMode(Modify);
        for(std::map<unsigned int,patientInfoQtCompentsPair>::iterator it = this->patternCompents->begin();
                                                                       it != this->patternCompents->end();
                                                                       it++){
            it->second.second->setEnabled(true);
            this->buttonOK->setText("OK");
            this->buttonCancle->setText("Cancel");
        }
    }
}

void PatientInput::fillInfo(const QString & value){
    QStringList list;
    QWidget* editContent = NULL;
    QString key = qobject_cast<QCompleter*>(sender())->objectName();
    DAO::getInstance()->getPatientInfoConnection()->getRowValueByItemValue_Patient(key,value,list);

    const std::map<unsigned int,OnePatientPattern>* pattern = ConfigLoader::getInstance()->getThePatientInfoPatten();

    if(!pattern || pattern->size() <=0){
        QMessageBox::critical(nullptr,"Error","There are not patient info pattern specialed!");
        exit(-1);
    }

    if(!this->patternCompents || this->patternCompents->size() != pattern->size()){
        QMessageBox::critical(nullptr,"Error","Internal error: pattern not matched!");
        exit(-1);
    }

    if((int)this->patternCompents->size() != list.size()){
        QMessageBox::critical(nullptr,"Error","Error: queried record not matched!");
        exit(-1);
    }

    for(std::map<unsigned int,OnePatientPattern>::const_iterator it = pattern->begin();
                                                                 it != pattern->end();
                                                                 it++){
        editContent = this->patternCompents->at(it->first).second;

        if("true" != it->second.supportPreQuery){
            dynamic_cast<QLineEdit*>(editContent)->setText(list.at(it->first));
        }else{
            dynamic_cast<QComboBox*>(editContent)->setCurrentText(list.at(it->first));
        }
    }
}

const std::map<unsigned int,QString>* PatientInput::getInfos() const{
    return this->infos;
}

PatientInputMode PatientInput::getCurrentMode() const{
    return this->currentMode;
}
