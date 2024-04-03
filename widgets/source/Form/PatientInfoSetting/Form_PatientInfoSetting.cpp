#include "../../../include/Form/PatientInfoSetting/Form_PatientInfoSetting.h"
#include "../../../include/Global/Config/Global_Config_ConfigLoader.h"
#include "ui_PatientInfoSetting.h"
#include <QPushButton>
#include <QPoint>
#include <QMessageBox>
#include <QCloseEvent>
#include <QLineEdit>
#include <QInputDialog>

PatientInfoSetting::PatientInfoSetting(QWidget* parent):QDialog(parent),uiForm(new Ui::PatientInfoSetting){

    uiForm->setupUi(this);

    this->setStyleSheet("background:#6fcade");

    uiForm->tableWidget->setEditTriggers(QAbstractItemView::DoubleClicked);

    uiForm->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    rightClickMenu = new QMenu(uiForm->tableWidget);

    QAction* action = new QAction("Insert_UP",rightClickMenu);
    rightClickMenu->addAction(action);
    QObject::connect(action,SIGNAL(triggered()),this,SLOT(insertUpAction()));

    action = new QAction("Insert_Down",rightClickMenu);
    rightClickMenu->addAction(action);
    QObject::connect(action,SIGNAL(triggered()),this,SLOT(insertDownAction()));

    action = new QAction("Delete",rightClickMenu);
    rightClickMenu->addAction(action);
    QObject::connect(action,SIGNAL(triggered()),this,SLOT(deleteAction()));

    action = new QAction("Reset",rightClickMenu);
    rightClickMenu->addAction(action);
    QObject::connect(action,SIGNAL(triggered()),this,SLOT(resetAction()));

    QObject::connect(uiForm->tableWidget,
                     SIGNAL(customContextMenuRequested(const QPoint &)),
                     this,
                     SLOT(execRightMenu(const QPoint &)));

    this->resetAction();
}

PatientInfoSetting::~PatientInfoSetting(){
    if(rightClickMenu){
        rightClickMenu->close();
        delete rightClickMenu;
        rightClickMenu = NULL;
    }
}

void PatientInfoSetting::closeEvent(QCloseEvent *event){
    bool OK = true;
    QString inputed;
    QMessageBox *msg = new QMessageBox();

    msg->setWindowTitle("Hint");
    msg->setStyleSheet("background: #8f91cc");
    msg->setInformativeText("Save the configuration?");
    QPushButton *buttonYes =  msg->addButton(tr("Yes and Close"),QMessageBox::ActionRole);
    buttonYes->setStyleSheet("QPushButton::default{border: 2px solid transparent;"
                             "color: #FFFFFF;"
                             "background-color: qlineargradient(spread:pad, x1:0.5, y1:0, x2:0.5, y2:1, stop:0 #84afe5, stop:1 #1168e4);}"
                             "QPushButton:hover{"
                             "color: #3d3d3d;}");
    QPushButton *buttonNo =  msg->addButton(tr("No but Close"),QMessageBox::ActionRole);
    msg->addButton(tr("Cancel but not Close"),QMessageBox::ActionRole);

    msg->exec();

    if(msg->clickedButton() == (QAbstractButton*)buttonYes){
        while(!(inputed = QInputDialog::getText(nullptr,
                                                 QString("Set confirm"),
                                                 QString("Please input 'hfimc'"),
                                                 QLineEdit::Password,
                                                 0,
                                                 &OK)).isEmpty() && OK && (inputed != QString("hfimc"))){
            QMessageBox::information(nullptr,"Error",QString("Input: %1, you should input 'hficm'").arg(inputed));
        }
        if(OK){
            bool setStatus = this->setPatientPattern();

            delete msg;
            msg = NULL;

            if(setStatus){
                QDialog::closeEvent(event);
            }else{
                event->ignore();
            }
        }else{
            delete msg;
            msg = NULL;

            event->ignore();
        }
    }else if(msg->clickedButton() == (QAbstractButton*)buttonNo){
        delete msg;
        msg = NULL;

        QDialog::closeEvent(event);
    }else{
        delete msg;
        msg = NULL;
        event->ignore();
    }
}

void PatientInfoSetting::insertUpAction(){
    int rowId = this->uiForm->tableWidget->currentRow();
    if(rowId < 0) rowId = 0;
    this->uiForm->tableWidget->insertRow(rowId);
    this->uiForm->tableWidget->update();
}

void PatientInfoSetting::insertDownAction(){
    int rowId = 0;
    if(this->uiForm->tableWidget->rowCount() > 0){
        rowId = this->uiForm->tableWidget->currentRow() + 1;
    }
    this->uiForm->tableWidget->insertRow(rowId);
    this->uiForm->tableWidget->update();
}

void PatientInfoSetting::deleteAction(){
    if(this->uiForm->tableWidget->rowCount() > 0){
        QList<QTableWidgetItem*> items = this->uiForm->tableWidget->selectedItems();
        for(QList<QTableWidgetItem*>::reverse_iterator it = items.rbegin(); it != items.rend(); ++it){
            this->uiForm->tableWidget->removeRow((*it)->row());
        }
    }
    this->uiForm->tableWidget->update();
}

void PatientInfoSetting::resetAction(){
    while(this->uiForm->tableWidget->rowCount()>0){
        this->uiForm->tableWidget->removeRow(0);
    }
    this->uiForm->tableWidget->setRowCount(0);

    const std::map<unsigned int,OnePatientPattern> *patientInfo = ConfigLoader::getInstance()->getThePatientInfoPatten();

    if(patientInfo){
        for(std::map<unsigned int,OnePatientPattern>::const_iterator it = patientInfo->begin();
                                                           it != patientInfo->end();
                                                           it++){
            this->uiForm->tableWidget->insertRow(it->first);
            this->uiForm->tableWidget->setItem(it->first,0,new QTableWidgetItem(it->second.labelName));
            this->uiForm->tableWidget->setItem(it->first,1,new QTableWidgetItem(it->second.infoName));
            this->uiForm->tableWidget->setItem(it->first,2,new QTableWidgetItem(it->second.necessary));
        }
    }
}

void PatientInfoSetting::execRightMenu(const QPoint & pos){
    this->rightClickMenu->exec(QCursor::pos());
}

bool PatientInfoSetting::setPatientPattern(){
    bool result = true;
    int count = this->uiForm->tableWidget->rowCount();
    QString Label;
    QString Name;
    QString Necessary;
    std::map<unsigned int,OnePatientPattern> patientPattern;
    for(int i = 0;i<count;i++){
        Label = this->uiForm->tableWidget->item(i,0)->text();
        Name = this->uiForm->tableWidget->item(i,1)->text();
        Necessary = this->uiForm->tableWidget->item(i,2)->text();

        if(Necessary != "true" && Necessary != "false"){
            QMessageBox::critical(nullptr,
                                  "Error",
                                  QString("The : %1 th patient info including wrong bool value setting: %2")
                                      .arg(i).arg(Necessary));
            result = false;
            break;
        }

        if(Name.count(' ') > 0){
            QMessageBox::critical(nullptr,"Error",QString("The Name %1 is not supported").arg(Name));
            result = false;
            break;
        }
        patientPattern.insert(
            std::pair<unsigned int,OnePatientPattern>(i,OnePatientPattern(Label,Name,Necessary))
            );
    }

    if(true == result){
        ConfigLoader::getInstance()->setThePatientPattern(patientPattern);
    }
    return result;
}
