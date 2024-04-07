#include "../../../include/Form/TRTimeOperator/Form_TRTimeOperator.h"
#include "ui_TRTimeOperator.h"
#include "../../../include/Form/TRTimeOperator/Form_TRTimeOperator_QueryForNextPatient.h"
#include "../../../include/Form/PatientInfoSetting/Form_PatientInfoSetting.h"
#include "../../../include/Form/OperationPipelineSetting/Form_OperationPipelineSetting.h"
#include "../../../include/Storage/DAO/Storage_DAO.h"
#include "../../../include/Storage/CSV/Storage_CSV_Writer.h"
#include "../../../include/Global/Config/Global_Config_ConfigLoader.h"
#include "../../../include/Global/Communication/Global_Communication_Record.h"
#include "../../../include/Common/Ui/Common_Ui_SelfPushButton.h"
#include "../../../include/Form/OperationLoop/Form_OperationLoop.h"
#include "../../../include/Form/OnlineDatabaseSetting/Form_OnlineDatabaseSetting.h"
#include <QMessageBox>
#include <QErrorMessage>
#include <QString>
#include <QDateTime>
#include <QDate>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QToolButton>
#include <QTabBar>
#include <QQuickView>
#include <QQuickWindow>
#include <QQuickWidget>
#include <QSqlTableModel>
#include <QTableView>
#include <QTableWidget>
#include <QFile>
#include <QStringList>
#include <QHeaderView>
#include <QScrollBar>
#include <QPushButton>
#include <QFrame>

TRTimeOperator::TRTimeOperator(TRTimeOperator_Interface* parent):TRTimeOperator_Interface(parent),uiForm(new Ui::TRTimeOperator){
    this->clear();
    uiForm->setupUi(this);
    this->uiConstruct();
    this->show();
    this->inputPatientInfo(PatientInputMode(Modify));
}

TRTimeOperator::~TRTimeOperator(){
    this->clear();
}

void TRTimeOperator::uiConstruct(){
    this->csvViewer = new CSVViewer();
    this->daoViewer = new DAOViewer();

    new QHBoxLayout(this); //registe a Layout object for current window
    this->layout()->setGeometry(this->geometry());

    this->menuBar = new OperatorMenuBar(this);
    this->menuBar->uiConstruct(QRect(0,
                                     0,
                                     this->geometry().width(),
                                     this->geometry().height()/25));

    QObject::connect(this->menuBar->getMenu("DataView")->actions().at(0),&QAction::triggered,this,&TRTimeOperator::csvView);
    QObject::connect(this->menuBar->getMenu("DataView")->actions().at(1),&QAction::triggered,this,&TRTimeOperator::dataBaseView);
    QObject::connect(this->menuBar->getMenu("Setting")->actions().at(0),
                     &QAction::triggered,
                     this,
                     &TRTimeOperator::storageSetting);
    QObject::connect(this->menuBar->getMenu("Setting")->actions().at(1),
                     &QAction::triggered,
                     this,
                     &TRTimeOperator::patientInfoSetting);
    QObject::connect(this->menuBar->getMenu("Setting")->actions().at(2),
                     &QAction::triggered,
                     this,
                     &TRTimeOperator::pipleLineSetting);
    QObject::connect(this->menuBar->getMenu("Setting")->actions().at(3),
                     &QAction::triggered,
                     this,
                     &TRTimeOperator::networkSetting);

    this->toolBar = new OperatorToolBar(this);
    this->toolBar->uiConstruct(QRect(0,
                                     this->geometry().height()/25,
                                     this->geometry().width(),
                                     this->geometry().height()/25));

    this->uiForm->tabWidget->setGeometry(0,
                                         this->toolBar->geometry().bottom(),
                                         this->geometry().width(),
                                         this->geometry().height() - this->toolBar->geometry().bottom());

    QObject::connect(this->uiForm->tabWidget,SIGNAL(tabCloseRequested(int)),this,SLOT(removeTable(int)));

    this->uiForm->tabWidget->setTabsClosable(true);
    this->uiForm->tabWidget->tabBar()->setTabButton(0,QTabBar::RightSide,nullptr);

    this->uiForm->quickWidget->setSource(QUrl("qrc:/Operator_BackGround.qml"));

    this->buttonConstruct();
}

void TRTimeOperator::uiDeconstruct(){
    if(this->menuBar){
        this->menuBar->uiDeConstruct();
        delete this->menuBar;
        this->menuBar = NULL;
    }

    if(this->toolBar){
        this->toolBar->uiDeConstruct();
        delete this->toolBar;
        this->toolBar = NULL;
    }

    if(this->buttonGroup){
        delete this->buttonGroup;
        this->buttonGroup = NULL;
    }

    if(this->pauseButton){
        this->pauseButton->close();
        delete this->pauseButton;
        this->pauseButton = NULL;
    }
}

void TRTimeOperator::buttonClear(){
    if(this->pauseButton){
        this->pauseButton->close();
        delete this->pauseButton;
        this->pauseButton = NULL;
    }

    if(this->buttonsMap){
        for(std::map<unsigned int,QAbstractButton*>::iterator it = this->buttonsMap->begin();
             it != this->buttonsMap->end();
             it++){
            it->second->close();
            delete it->second;
            it->second = NULL;
        }

        std::map<unsigned int,QAbstractButton*>().swap(*this->buttonsMap);
        this->buttonsMap->clear();
        delete this->buttonsMap;
        this->buttonsMap = NULL;
    }

    if(this->buttonGroup){
        QObject::disconnect(this->buttonGroup,SIGNAL(idClicked(int)),this,SLOT(HandleSignal(int)));
        delete this->buttonGroup;
        this->buttonGroup = NULL;
    }

    if(this->linesMap){
        for(std::map<unsigned int,QFrame*>::iterator it = this->linesMap->begin();
                                                     it != this->linesMap->end();
                                                     it++){
            it->second->close();
            delete it->second;
            it->second = NULL;
        }

        std::map<unsigned int,QFrame*>().swap(*this->linesMap);
        this->linesMap->clear();
        delete this->linesMap;
        this->linesMap = NULL;
    }

    std::map<unsigned int,QString>().swap(Record::getInstance()->buttonTimeRecord);
    Record::getInstance()->buttonTimeRecord.clear();
}

void TRTimeOperator::buttonConstruct(){
    this->buttonClear();
    /*Registe button to buttonGroup for clicked id get and regist button to map*/
    /*To do: we need to config button based on configure file*/
    QPushButton* button = NULL;
    QFrame* line = NULL;
    QString lineColor = "rgb(0, 254, 255)";
    double buttonWidth = this->width()*0.2;
    double buttonHeight = this->height()*0.1;
    double buttonTop = buttonHeight*0.7;
    double buttonLeft = this->width()/2 - buttonWidth*0.5;
    double lineTop = buttonTop + buttonHeight;
    double lineWidth = buttonWidth*0.1;
    double lineLeft = this->width()/2 - lineWidth*0.5;
    double lineHeight = buttonHeight;
    double buttonRadio = buttonWidth*0.6;
    QSize pushButtonSize(buttonRadio,buttonRadio);
    this->buttonGroup = new QButtonGroup(this->uiForm->quickWidget);
    this->buttonsMap = new std::map<unsigned int,QAbstractButton*>();
    this->linesMap = new std::map<unsigned int,QFrame*>();
    double dynamicHeight = buttonTop + buttonHeight*2*ConfigLoader::getInstance()->getTheOperationPatten()->size();

    this->uiForm->quickWidget->setMinimumHeight(dynamicHeight);
    this->uiForm->quickWidget->setMinimumWidth(this->uiForm->tabWidget->width());

    for(std::map<unsigned int,OneOperationPattern>::const_iterator it = ConfigLoader::getInstance()->getTheOperationPatten()->begin();
                                                                   it != ConfigLoader::getInstance()->getTheOperationPatten()->end();
                                                                   it++){
        button = new QPushButton(this->uiForm->quickWidget);

        button->setText(it->second.buttonLabel);
        button->setGeometry(buttonLeft,buttonTop,buttonWidth,buttonHeight);

        this->buttonGroup->addButton(button,it->first);
        this->buttonsMap->insert(std::pair<unsigned int,QAbstractButton*>(it->first,button));

        button->show();

        buttonTop += buttonHeight*2;

        if(it->first != (ConfigLoader::getInstance()->getTheOperationPatten()->size() - 1)){
            line = new QFrame(this->uiForm->quickWidget);
            line->setGeometry(lineLeft,lineTop,lineWidth,lineHeight);
            line->setStyleSheet(QString("color: %1;"
                                    "selection-color: %1;"
                                    "border-bottom-color: %1;"
                                    "border-right-color: %1;"
                                    "border-top-color: %1;"
                                    "gridline-color: %1;"
                                    "border-color: %1;").arg(lineColor));

            linesMap->insert(std::pair<unsigned int,QFrame*>(it->first,line));

            line->show();

            lineTop += lineHeight*2;
        }
    }

    this->buttonGroup->setExclusive(true);

    this->changeButtonStatus(0);

    QObject::connect(this->buttonGroup,SIGNAL(idClicked(int)),this,SLOT(HandleSignal(int)));

    /*Pause button*/
    this->pauseButton = new QPushButton(this->uiForm->tabWidget->widget(0));
    this->pauseButton->setToolTip("Pause");
    this->pauseButton->setIcon(QIcon(":/img/Pause.svg"));
    this->pauseButton->setIconSize(pushButtonSize*1.05);
    this->pauseButton->setMask(QPixmap(":/img/cicle.svg").scaled(pushButtonSize).mask());
    this->pauseButton->setGeometry(this->width()*0.5 + buttonWidth*0.5 + 0.5*(this->width()*0.5 - buttonWidth*0.5) - pushButtonSize.width()*0.5,
                                   0.5*(this->height() - this->menuBar->height() - this->toolBar->height()) - pushButtonSize.height()*0.5,
                                   pushButtonSize.width(),
                                   pushButtonSize.height());
    this->pauseButton->show();

    QObject::connect(this->pauseButton,SIGNAL(pressed()),this,SLOT(HandlePauseSignal()));
}

void TRTimeOperator::clear(){
    this->uiDeconstruct();

    if(this->buttonsMap){
        std::map<unsigned int,QAbstractButton*>().swap(*this->buttonsMap);
        this->buttonsMap->clear();
        delete this->buttonsMap;
        this->buttonsMap = NULL;
    }

    if(this->linesMap){
        for(std::map<unsigned int,QFrame*>::iterator it = this->linesMap->begin();
             it != this->linesMap->end();
             it++){
            it->second->close();
            delete it->second;
            it->second = NULL;
        }

        std::map<unsigned int,QFrame*>().swap(*this->linesMap);
        this->linesMap->clear();
        delete this->linesMap;
        this->linesMap = NULL;
    }

    Record::getInstance()->clear();

    if(this->csvViewer){
        this->csvViewer->clear();
        delete this->csvViewer;
        this->csvViewer = NULL;
    }

    if(this->daoViewer){
        this->daoViewer->clear();
        delete this->daoViewer;
        this->daoViewer = NULL;
    }
}

void TRTimeOperator::HandleSignal(int ID){
    this->menuBar->getMenu("Setting")->actions().at(2)->setEnabled(false);

    if(0 == ID){
        this->inputPatientInfo(PatientInputMode(ViewAndModify));
    }

    OperationLoop * operationForm = new OperationLoop(this,ID);
    operationForm->exec();

    delete operationForm;
    operationForm = NULL;

    if((unsigned int)(ID +1) == this->buttonsMap->size()){
        std::map<unsigned int,std::pair<QString,QString>>().swap(Record::getInstance()->patientInfoRecord);
        Record::getInstance()->patientInfoRecord.clear();

        std::map<unsigned int,QString>().swap(Record::getInstance()->buttonTimeRecord);
        Record::getInstance()->buttonTimeRecord.clear();

        this->queryForNextPatient();
        this->menuBar->getMenu("Setting")->actions().at(2)->setEnabled(true);
    }

    this->changeButtonStatus(ID+1);
}

void TRTimeOperator::HandlePauseSignal(){
    if("Pause" == this->pauseButton->toolTip()){
        this->pauseButton->setIcon(QIcon(":/img/Continue.svg"));
        this->uiForm->quickWidget->setEnabled(false);
        this->menuBar->setEnabled(false);
        this->toolBar->setEnabled(false);
        this->pauseButton->setToolTip("Continue");
    }else{
        this->pauseButton->setIcon(QIcon(":/img/Pause.svg"));
        this->uiForm->quickWidget->setEnabled(true);
        this->menuBar->setEnabled(true);
        this->toolBar->setEnabled(true);
        this->pauseButton->setToolTip("Pause");
    }
}

void TRTimeOperator::removeTable(int index){
    QWidget* widget = this->uiForm->tabWidget->widget(index);

    if(daoViewer){
        if(daoViewer->viewTree){
            daoViewer->deleteOneLeaf(dynamic_cast<QTabWidget*>(widget));
        }
    }

    if(csvViewer){
        if(csvViewer->viewTree){
            csvViewer->deleteOneLeaf(dynamic_cast<QTabWidget*>(widget));
        }
    }

    this->uiForm->tabWidget->removeTab(index);
}

void TRTimeOperator::csvView(){
    QString line;
    QStringList headerValues;
    QStringList ItemValues;
    unsigned long rowId = 0;
    QTabWidget* tempWidgetTable = new QTabWidget(this->uiForm->tabWidget);
    OneCSVViewer* oneCSVViewer = new OneCSVViewer();
    QFile fileReader(systemCSVPath);

    csvViewer->insertOneLeaf(tempWidgetTable,oneCSVViewer);

    tempWidgetTable->setGeometry(0,
                                 0,
                                 this->uiForm->tabWidget->size().width(),
                                 this->uiForm->tabWidget->size().height());

    this->uiForm->tabWidget->addTab(tempWidgetTable,"CSV Viewer");

    tempWidgetTable->setTabsClosable(true);
    tempWidgetTable->setTabPosition(QTabWidget::South);
    tempWidgetTable->setDocumentMode(true);

    fileReader.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream stream(&fileReader);

    if(fileReader.isOpen()){
        QWidget * tempWidget = NULL;
        OneCSVViewerCompents * tempOneCSVViewerCompents = NULL;

        while(!stream.atEnd()){
            line = stream.readLine();

            if('#' == line.at(0)){
                tempWidget = new QWidget(tempWidgetTable);
                tempWidgetTable->insertTab(0,tempWidget,line);

                tempWidget->setGeometry(0,
                                        0,
                                        tempWidgetTable->width(),
                                        tempWidgetTable->height());

                tempOneCSVViewerCompents = new OneCSVViewerCompents();

                oneCSVViewer->insertOneLeaf(tempWidget,tempOneCSVViewerCompents);

                tempOneCSVViewerCompents->tableView = new QTableWidget(tempWidget);

                if(!stream.atEnd()){
                    line = stream.readLine();
                }else{
                    break;
                }

                ItemValues = line.split(',');

                QStringList().swap(headerValues);
                headerValues.clear();

                for(unsigned int colID = 0; colID<ItemValues.size(); colID++){
                    headerValues<<ItemValues[colID].split(" ").back();
                }

                tempOneCSVViewerCompents->tableView->setColumnCount(ItemValues.size());

                tempOneCSVViewerCompents->tableView->setRowCount(totalShowCSVLine);

                tempOneCSVViewerCompents->tableView->setGeometry(0,
                                                                 0,
                                                                 tempWidget->size().width() - this->uiForm->tabWidget->tabBar()->width(),
                                                                 tempWidget->size().height());

                tempOneCSVViewerCompents->tableView->setStyleSheet(QString("background:'#498a78'"));

                tempOneCSVViewerCompents->tableView->setHorizontalHeaderLabels(headerValues);
                //tempOneCSVViewerCompents->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
                tempOneCSVViewerCompents->tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
                tempOneCSVViewerCompents->tableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
                tempOneCSVViewerCompents->tableView->verticalScrollBar()->setHidden(false);

                rowId = 0;
            }else{
                if(rowId < totalShowCSVLine){
                    ItemValues = line.split(',');

                    for(unsigned int colID = 0; colID<ItemValues.size(); colID++){
                        QTableWidgetItem *tempItem = new QTableWidgetItem(ItemValues[colID].split(" ").back());
                        tempOneCSVViewerCompents->tableView->setItem(rowId,colID,tempItem);
                    }

                    rowId++;
                }
            }
        }
        tempOneCSVViewerCompents->tableView->update();

        tempOneCSVViewerCompents->tableView->show();
    }else{
        QMessageBox::critical(nullptr,"Error",QString("The csv file %1 cannot be opened").arg(systemCSVPath));
        exit(-1);
    }

    fileReader.close();
}

void TRTimeOperator::dataBaseView(){
    this->dataBaseView_TR();
    this->dataBaseView_PatientInfo();
}

void TRTimeOperator::dataBaseView_TR(){
    QTabWidget* tempWidgetTable = new QTabWidget(this->uiForm->tabWidget);
    OneDataTableViewer* oneDataTableViewer = new OneDataTableViewer();
    daoViewer->insertOneLeaf(tempWidgetTable,oneDataTableViewer);

    tempWidgetTable->setGeometry(0,
                                 0,
                                 this->uiForm->tabWidget->size().width(),
                                 this->uiForm->tabWidget->size().height());

    this->uiForm->tabWidget->addTab(tempWidgetTable,"DataBase_TR");

    tempWidgetTable->setTabsClosable(true);
    tempWidgetTable->setTabPosition(QTabWidget::South);
    tempWidgetTable->setDocumentMode(true);

    const std::list<QString> & tables = DAO::getInstance()->getTrInfoConnection()->getAllTablesName();
    for(std::list<QString>::const_reverse_iterator it = tables.rbegin();
                                                   it != tables.rend();
                                                   ++it){

        if(*it == patientInfo_TableName || *it == (patientInfo_TableName + "_back")){
            continue;
        }

        QWidget* tempWidget = new QWidget(tempWidgetTable);

        OneDataTableViewerCompents * compents = new OneDataTableViewerCompents();
        oneDataTableViewer->insertOneLeaf(tempWidget,compents);

        tempWidget->setGeometry(0,
                                0,
                                tempWidgetTable->width(),
                                tempWidgetTable->height());

        tempWidgetTable->addTab(tempWidget,*it);

        compents->tableView = new QTableView(tempWidget);
        compents->model = new QSqlTableModel(tempWidget);

        compents->model->setTable(*it);
        compents->model->setQuery(QString("select * from %1").arg(*it),*DAO::getInstance()->getTrInfoConnection()->getTheDataBase());

        compents->tableView->setModel(compents->model);
        compents->tableView->setGeometry(0,
                                         0,
                                         tempWidget->size().width(),
                                         tempWidget->size().height()*0.9);

        compents->tableView->setEditTriggers(QAbstractItemView::DoubleClicked);

        compents->insertButton = new SelfPushButton(tempWidget);
        //bind a shortcut
        this->daoViewer->bindOneButtonToSqlModel(compents->insertButton,compents->model);
        this->daoViewer->bindOneButtonToTableView(compents->insertButton,compents->tableView);
        compents->insertButton->setText("Insert record");
        compents->insertButton->setStyleSheet("background:#07343e");
        compents->insertButton->setObjectName(compents->insertButton->text());
        compents->insertButton->setGeometry(tempWidget->size().width()*0.2,
                                            tempWidget->size().height()*0.91,
                                            tempWidget->size().width()*0.2,
                                            tempWidget->size().height()*0.04);
        QObject::connect(compents->insertButton,SIGNAL(sign_handlePressEvent(QObject*)),this->daoViewer,SLOT(appendARow(QObject*)));

        compents->deleteButton = new SelfPushButton(tempWidget);
        //bind a shortcut
        this->daoViewer->bindOneButtonToSqlModel(compents->deleteButton,compents->model);
        this->daoViewer->bindOneButtonToTableView(compents->deleteButton,compents->tableView);
        compents->deleteButton->setText("Delete record");
        compents->deleteButton->setStyleSheet("background:#07343e");
        compents->deleteButton->setObjectName(compents->deleteButton->text());
        compents->deleteButton->setGeometry(tempWidget->size().width()*0.6,
                                            tempWidget->size().height()*0.91,
                                            tempWidget->size().width()*0.2,
                                            tempWidget->size().height()*0.04);
        QObject::connect(compents->deleteButton,SIGNAL(sign_handlePressEvent(QObject*)),this->daoViewer,SLOT(deleteRow(QObject*)));

        compents->tableView->setStyleSheet(QString("background:'#498a78'"));
        compents->tableView->show();
    }

    /*
    table->record();
    int count = table->rowCount();
    table->insertRecord(count,)
    */
}


void TRTimeOperator::dataBaseView_PatientInfo(){
    QTabWidget* tempWidgetTable = new QTabWidget(this->uiForm->tabWidget);
    OneDataTableViewer* oneDataTableViewer = new OneDataTableViewer();
    daoViewer->insertOneLeaf(tempWidgetTable,oneDataTableViewer);

    tempWidgetTable->setGeometry(0,
                                 0,
                                 this->uiForm->tabWidget->size().width(),
                                 this->uiForm->tabWidget->size().height());

    this->uiForm->tabWidget->addTab(tempWidgetTable,"DataBase_PatientInfo");

    tempWidgetTable->setTabsClosable(true);
    tempWidgetTable->setTabPosition(QTabWidget::South);
    tempWidgetTable->setDocumentMode(true);

    const std::list<QString> & tables = DAO::getInstance()->getPatientInfoConnection()->getAllTablesName();
    for(std::list<QString>::const_reverse_iterator it = tables.rbegin();
         it != tables.rend();
         ++it){

        if(*it != patientInfo_TableName){
            continue;
        }

        QWidget* tempWidget = new QWidget(tempWidgetTable);

        OneDataTableViewerCompents * compents = new OneDataTableViewerCompents();
        oneDataTableViewer->insertOneLeaf(tempWidget,compents);

        tempWidget->setGeometry(0,
                                0,
                                tempWidgetTable->width(),
                                tempWidgetTable->height());

        tempWidgetTable->addTab(tempWidget,*it);

        compents->tableView = new QTableView(tempWidget);
        compents->model = new QSqlTableModel(tempWidget);

        compents->model->setTable(*it);
        compents->model->setQuery(QString("select * from %1").arg(*it),*DAO::getInstance()->getPatientInfoConnection()->getTheDataBase());

        compents->tableView->setModel(compents->model);
        compents->tableView->setGeometry(0,
                                         0,
                                         tempWidget->size().width(),
                                         tempWidget->size().height()*0.9);

        compents->tableView->setEditTriggers(QAbstractItemView::DoubleClicked);

        compents->insertButton = new SelfPushButton(tempWidget);
        //bind a shortcut
        this->daoViewer->bindOneButtonToSqlModel(compents->insertButton,compents->model);
        this->daoViewer->bindOneButtonToTableView(compents->insertButton,compents->tableView);
        compents->insertButton->setText("Insert record");
        compents->insertButton->setStyleSheet("background:#07343e");
        compents->insertButton->setObjectName(compents->insertButton->text());
        compents->insertButton->setGeometry(tempWidget->size().width()*0.2,
                                            tempWidget->size().height()*0.91,
                                            tempWidget->size().width()*0.2,
                                            tempWidget->size().height()*0.04);
        QObject::connect(compents->insertButton,SIGNAL(sign_handlePressEvent(QObject*)),this->daoViewer,SLOT(appendARow(QObject*)));

        compents->deleteButton = new SelfPushButton(tempWidget);
        //bind a shortcut
        this->daoViewer->bindOneButtonToSqlModel(compents->deleteButton,compents->model);
        this->daoViewer->bindOneButtonToTableView(compents->deleteButton,compents->tableView);
        compents->deleteButton->setText("Delete record");
        compents->deleteButton->setStyleSheet("background:#07343e");
        compents->deleteButton->setObjectName(compents->deleteButton->text());
        compents->deleteButton->setGeometry(tempWidget->size().width()*0.6,
                                            tempWidget->size().height()*0.91,
                                            tempWidget->size().width()*0.2,
                                            tempWidget->size().height()*0.04);
        QObject::connect(compents->deleteButton,SIGNAL(sign_handlePressEvent(QObject*)),this->daoViewer,SLOT(deleteRow(QObject*)));

        compents->tableView->setStyleSheet(QString("background:'#498a78'"));
        compents->tableView->show();
    }
}

void TRTimeOperator::storageSetting(){
    QMessageBox::information(nullptr,"Info","789");
}

void TRTimeOperator::patientInfoSetting(){
    PatientInfoSetting *form = new PatientInfoSetting(this);
    form->exec();

    delete form;
    form = NULL;
}

void TRTimeOperator::pipleLineSetting(){
    OperationPipelineSetting *form = new OperationPipelineSetting(this);
    form->exec();

    this->buttonConstruct();

    delete form;
    form = NULL;
}

void TRTimeOperator::networkSetting(){
    OnlineDatabaseSetting *databaseSettingForm = new OnlineDatabaseSetting(this);
    databaseSettingForm->exec();

    DAO::getInstance()->patientInfoReConnect();

    delete databaseSettingForm;
    databaseSettingForm = NULL;
}

void TRTimeOperator::changeButtonStatus(unsigned int buttonID){
    if(0 >= this->buttonsMap->size()){
        QMessageBox::critical(nullptr, "Error", "None button registerd!");
        exit(-1);
    }

    if(buttonID < this->buttonsMap->size()){
        std::map<unsigned int,QAbstractButton*>::iterator it = this->buttonsMap->begin();

        for(;it != this->buttonsMap->end(); it++){
            if(it->first != (unsigned int)buttonID){
                it->second->setEnabled(false);
                it->second->setStyleSheet("background-color:grey");
            }else{
                it->second->setEnabled(true);
                it->second->setStyleSheet("background-color:green");
            }
        }
    }else{
        if((unsigned int)buttonID == this->buttonsMap->size()){
            this->buttonsMap->at(buttonID-1)->setEnabled(false);
            this->buttonsMap->at(buttonID-1)->setStyleSheet("background-color:grey");
        }else{
            QMessageBox::critical(nullptr, "Error", "Button ID exceed!");
            exit(-1);
        }
    }
}

void TRTimeOperator::queryForNextPatient(){
    QueryNextPatientDialog *dialog = new QueryNextPatientDialog(this);
    /*
    QueryNextPatientDialog (this);                  //Please do not allocate the QDialog in stack when some compents within
                                                    //it are allocated in heap, because it would not be released automate
                                                    //, besides, the Qt::WA_DeleteOnClose flag is also not available(see the)
                                                    //(explain below).

    dialog.setAttribute(Qt::WA_DeleteOnClose);      //Please call this function careful, beacause
                                                    //if some compents within the object and allocated in heap
                                                    //would be released manually when deconstructor are called,
                                                    //but this flag would also release the compents, so
                                                    //the compents would be release two times, so the memory error would
                                                    //occur.
    */
    int code = dialog->exec();
    if(QDialog::Accepted == code){
        this->inputPatientInfo(PatientInputMode(Modify));
        this->changeButtonStatus(0);
    }else if(QDialog::Rejected == code){
        bool OK = true;
        QString inputed;
        while(!(inputed = QInputDialog::getText(this,QString("Delete confirm"),QString("Please input 'hfimc'"),QLineEdit::Password,0,&OK)).isEmpty() && OK && (inputed != QString("hfimc"))){
            QMessageBox::information(nullptr,"Error",QString("Input: %1, you should input 'hficm'").arg(inputed));
        }
        if(OK){
            DAO::getInstance()->getTrInfoConnection()->deleteLastRecord(Record::getInstance()->lastTableName);
            CSVWriter::getInstance()->deleteLastRecord();
        }
        this->inputPatientInfo(PatientInputMode(Modify));
        this->changeButtonStatus(0);
    }else{
        this->inputPatientInfo(PatientInputMode(Modify));
        this->changeButtonStatus(0);
    }

    if(dialog){
        delete dialog;
        dialog = NULL;
    }
}

void TRTimeOperator::inputPatientInfo(PatientInputMode model){
    QString InfoName;
    PatientInput * patientForm = new PatientInput(this,&Record::getInstance()->patientInfoRecord,model);
    int result = patientForm->exec();
    const std::map<unsigned int,QString>* info = patientForm->getInfos();

    if(PatientInputMode(Modify) == patientForm->getCurrentMode()){
        std::map<unsigned int,std::pair<QString,QString>>().swap(Record::getInstance()->patientInfoRecord);
        Record::getInstance()->patientInfoRecord.clear();
        for(std::map<unsigned int,QString>::const_iterator it = info->begin();
                                                           it != info->end();
                                                           it++){
            InfoName = ConfigLoader::getInstance()->getThePatientInfoPatten()->at(it->first).infoName;
            Record::getInstance()->patientInfoRecord.insert(
                std::pair<unsigned int,std::pair<QString,QString>>(
                    it->first,
                    std::pair<QString,QString>(InfoName,it->second)
                )
            );
        }

        if(QDialog::Accepted == result){
            if(DAO::getInstance()->getPatientInfoConnection()->needToUpdateTable_Patient(*ConfigLoader::getInstance()->getThePatientInfoPatten())){
                DAO::getInstance()->getPatientInfoConnection()->updateTable_Patient();
            }

            DAO::getInstance()->getPatientInfoConnection()->appendARow_Patient(Record::getInstance()->patientInfoRecord);
        }
    }
    delete patientForm;
    patientForm = NULL;
}
