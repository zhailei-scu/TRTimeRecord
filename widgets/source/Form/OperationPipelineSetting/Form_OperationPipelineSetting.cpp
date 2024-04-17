#include "../../../include/Form/OperationPipelineSetting/Form_OperationPipelineSetting.h"
#include "../../../include/Global/Config/Global_Config_ConfigLoader.h"
#include "ui_OperationPipelineSetting.h"
#include <QPushButton>
#include <QPoint>
#include <QMessageBox>
#include <QCloseEvent>
#include <QLineEdit>
#include <QInputDialog>
#include <sstream>
#include <QComboBox>
#include <QTextEdit>
#include <QTextBlock>

OperationPipelineSetting::OperationPipelineSetting(QWidget* parent):QDialog(parent),uiForm(new Ui::OperationPipelineSetting){

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

OperationPipelineSetting::~OperationPipelineSetting(){
    if(rightClickMenu){
        rightClickMenu->close();
        delete rightClickMenu;
        rightClickMenu = NULL;
    }
}

void OperationPipelineSetting::closeEvent(QCloseEvent *event){
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
            bool setStatus = this->setOperationPipelineSetting();

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

void OperationPipelineSetting::insertUpAction(){
    int rowId = this->uiForm->tableWidget->currentRow();
    if(rowId < 0) rowId = 0;
    this->uiForm->tableWidget->insertRow(rowId);
    this->uiForm->tableWidget->update();
}

void OperationPipelineSetting::insertDownAction(){
    int rowId = 0;
    if(this->uiForm->tableWidget->rowCount() > 0){
        rowId = this->uiForm->tableWidget->currentRow() + 1;
    }
    this->uiForm->tableWidget->insertRow(rowId);
    this->uiForm->tableWidget->update();
}

void OperationPipelineSetting::deleteAction(){
    if(this->uiForm->tableWidget->rowCount() > 0){
        QList<QTableWidgetItem*> items = this->uiForm->tableWidget->selectedItems();
        for(QList<QTableWidgetItem*>::reverse_iterator it = items.rbegin(); it != items.rend(); ++it){
            this->uiForm->tableWidget->removeRow((*it)->row());
        }
    }
    this->uiForm->tableWidget->update();
}

void OperationPipelineSetting::resetAction(){
    std::stringstream ss;
    while(this->uiForm->tableWidget->rowCount()>0){
        this->uiForm->tableWidget->removeRow(0);
    }
    this->uiForm->tableWidget->setRowCount(0);

    const std::map<unsigned int,OneOperationPattern> *operation = ConfigLoader::getInstance()->getTheOperationPatten();

    if(operation){
        for(std::map<unsigned int,OneOperationPattern>::const_iterator it = operation->begin();
                                                                       it != operation->end();
                                                                       it++){
            ss.str("");
            ss.clear();
            ss<<it->second.repeatTime;

            this->uiForm->tableWidget->insertRow(it->first);
            this->uiForm->tableWidget->setItem(it->first,0,new QTableWidgetItem(it->second.buttonLabel));
            this->uiForm->tableWidget->setItem(it->first,1,new QTableWidgetItem(it->second.buttonName));
            this->uiForm->tableWidget->setItem(it->first,2,new QTableWidgetItem(ss.str().c_str()));

            //QPushButton *button = new QPushButton(this->uiForm->tableWidget);
            QTextEdit* editor = new QTextEdit();

            for(std::map<unsigned int,QString>::const_iterator it_info = it->second.hintInfos.begin();
                                                               it_info != it->second.hintInfos.end();
                                                               it_info++){
                editor->append(it_info->second);
            }

            this->uiForm->tableWidget->setCellWidget(it->first,3,editor);
        }
    }
}

void OperationPipelineSetting::execRightMenu(const QPoint & pos){
    this->rightClickMenu->exec(QCursor::pos());
}

bool OperationPipelineSetting::setOperationPipelineSetting(){
    bool result = true;
    int count = this->uiForm->tableWidget->rowCount();
    QString Label;
    QString Name;
    int repeatTime;
    std::map<unsigned int,OneOperationPattern> opereation;
    std::map<unsigned int,QString> oneInfos;
    for(int i = 0;i<count;i++){
        Label = this->uiForm->tableWidget->item(i,0)->text();
        Name = this->uiForm->tableWidget->item(i,1)->text();
        repeatTime = this->uiForm->tableWidget->item(i,2)->text().toInt();

        std::map<unsigned int,QString>().swap(oneInfos);
        oneInfos.clear();

        QTextEdit* editor = dynamic_cast<QTextEdit*>(this->uiForm->tableWidget->cellWidget(i,3));

        if(editor && editor->document()){
            for(int iline = 0;iline<editor->document()->lineCount();iline++){
                oneInfos.insert(std::pair<unsigned int,QString>(iline,editor->document()->findBlockByLineNumber(iline).text()));
            }
        }

        if(Name.count(' ') > 0){
            QMessageBox::critical(nullptr,"Error",QString("The Name %1 is not supported").arg(Name));
            result = false;
            break;
        }

        if(0 == repeatTime || (repeatTime < 0 && -1 != repeatTime)){
            QMessageBox::critical(nullptr,
                                  "Error",
                                  QString("The json file for operation pipeline input id: %1 included wrong repeat time %2:"
                                          "The repeatTime can only be -1(represent nolimitation) or positive integer value")
                                      .arg(i).arg(repeatTime));
            result = false;
            break;
        }

        opereation.insert(std::pair<unsigned int,OneOperationPattern>(i,OneOperationPattern(Label,Name,repeatTime,oneInfos)));
    }

    ConfigLoader::getInstance()->setTheOperationPattern(opereation);
    return result;
}
