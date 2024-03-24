#include "../../../include/Form/TRTimeOperator/Form_TRTimeOperator.h"
#include "ui_TRTimeOperator.h"
#include "../../../include/Form/TRTimeOperator/Form_TRTimeOperator_QueryForNextPatient.h"
#include "../../../include/Form/PatientInfoSetting/Form_PatientInfoSetting.h"
#include "../../../include/Storage/DAO/Storage_DAO.h"
#include "../../../include/Storage/CSV/Storage_CSV_Writer.h"
#include "../../../include/Config/Config_ConfigLoader.h"
#include "../../../include/Common/Util/Common_Util_Base.h"
#include "../../../include/Common/Ui/Common_Ui_SelfPushButton.h"
#include <QMessageBox>
#include <QErrorMessage>
#include <QString>
#include <algorithm>
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
}

void TRTimeOperator::buttonConstruct(){
    /*Registe button to buttonGroup for clicked id get and regist button to map*/
    /*To do: we need to config button based on configure file*/
    this->buttonGroup = new QButtonGroup(this);
    this->buttonsMap = new std::map<unsigned int,QAbstractButton*>();

    const auto begin = ConfigLoader::getInstance()->getTheOperatorPatten()->begin();
    const auto end = ConfigLoader::getInstance()->getTheOperatorPatten()->end();

    auto it = std::find_if(begin,end,map_value_finder<unsigned int,QString>("PatientComeIn"));
    if(it == end){
        QMessageBox::critical(nullptr, "Error", QString("Button patten %1 is not found").arg("PatentComeIn"));
        exit(-1);
    }
    this->buttonGroup->addButton(this->uiForm->pushButton_PatentComeIn,it->first);
    this->buttonsMap->insert(std::pair<unsigned int,QAbstractButton*>(it->first,this->uiForm->pushButton_PatentComeIn));

    it = std::find_if(begin,end,map_value_finder<unsigned int,QString>("PatentImaging"));
    if(it == end){
        QMessageBox::critical(nullptr, "Error", QString("Button patten %1 is not found").arg("PatentImaging"));
        exit(-1);
    }
    this->buttonGroup->addButton(this->uiForm->pushButton_PatentImaging,it->first);
    this->buttonsMap->insert(std::pair<unsigned int,QAbstractButton*>(it->first,this->uiForm->pushButton_PatentImaging));

    it = std::find_if(begin,end,map_value_finder<unsigned int,QString>("Theraphy"));
    if(it == end){
        QMessageBox::critical(nullptr, "Error", QString("Button patten %1 is not found").arg("Theraphy"));
        exit(-1);
    }
    this->buttonGroup->addButton(this->uiForm->pushButton_Theraphy,it->first);
    this->buttonsMap->insert(std::pair<unsigned int,QAbstractButton*>(it->first,this->uiForm->pushButton_Theraphy));

    it = std::find_if(begin,end,map_value_finder<unsigned int,QString>("LeavingRoom"));
    if(it == end){
        QMessageBox::critical(nullptr, "Error", QString("Button patten %1 is not found").arg("LeavingRoom"));
        exit(-1);
    }
    this->buttonGroup->addButton(this->uiForm->pushButton_LeavingRoom,it->first);
    this->buttonsMap->insert(std::pair<unsigned int,QAbstractButton*>(it->first,this->uiForm->pushButton_LeavingRoom));

    this->buttonGroup->setExclusive(true);

    this->changeButtonStatus(0);

    QObject::connect(this->buttonGroup,SIGNAL(idClicked(int)),this,SLOT(HandleSignal(int)));
}

void TRTimeOperator::clear(){
    this->uiDeconstruct();

    if(this->buttonsMap){
        std::map<unsigned int,QAbstractButton*>().swap(*this->buttonsMap);
        this->buttonsMap->clear();
        delete this->buttonsMap;
        this->buttonsMap = NULL;
    }

    std::map<unsigned int,QString>().swap(this->patientInfoRecord);
    this->patientInfoRecord.clear();

    std::map<unsigned int,QString>().swap(this->buttonTimeRecord);
    this->buttonTimeRecord.clear();

    this->lastTableName = "";

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
    bool recorded = this->timeRecord(ID);

    this->changeButtonStatus(ID+1);

    if(recorded){
        this->queryForNextPatient();
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

        tempWidgetTable->addTab(tempWidget,line);

        while(!stream.atEnd()){
            line = stream.readLine();

            if('#' == line.at(0)){
                tempWidget = new QWidget(tempWidgetTable);

                tempWidget->setGeometry(0,
                                        0,
                                        tempWidgetTable->width(),
                                        tempWidgetTable->height());

                tempOneCSVViewerCompents = new OneCSVViewerCompents();

                oneCSVViewer->insertOneLeaf(tempWidget,tempOneCSVViewerCompents);

                tempOneCSVViewerCompents->tableView = new QTableWidget(tempWidget);

                tempOneCSVViewerCompents->tableView->setColumnCount(ConfigLoader::getInstance()->getThePatientInfoPatten()->size()
                                                                    +ConfigLoader::getInstance()->getTheOperatorPatten()->size());

                tempOneCSVViewerCompents->tableView->setRowCount(totalShowCSVLine);

                tempOneCSVViewerCompents->tableView->setGeometry(0,
                                                                 0,
                                                                 tempWidget->size().width() - this->uiForm->tabWidget->tabBar()->width(),
                                                                 tempWidget->size().height());

                tempOneCSVViewerCompents->tableView->setStyleSheet(QString("background:'#498a78'"));

                QStringList().swap(headerValues);
                headerValues.clear();

                for(std::map<unsigned int,patientInfoPair>::const_iterator it = ConfigLoader::getInstance()->getThePatientInfoPatten()->begin();
                                                                   it != ConfigLoader::getInstance()->getThePatientInfoPatten()->end();
                                                                   it++){
                    headerValues << it->second.second;
                }

                for(std::map<unsigned int,QString>::const_iterator it = ConfigLoader::getInstance()->getTheOperatorPatten()->begin();
                                                                   it != ConfigLoader::getInstance()->getTheOperatorPatten()->end();
                                                                   it++){
                    headerValues << it->second;
                }

                tempOneCSVViewerCompents->tableView->setHorizontalHeaderLabels(headerValues);
                //tempOneCSVViewerCompents->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
                tempOneCSVViewerCompents->tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
                tempOneCSVViewerCompents->tableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
                tempOneCSVViewerCompents->tableView->verticalScrollBar()->setHidden(false);

                tempWidgetTable->addTab(tempWidget,line);

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
    QTabWidget* tempWidgetTable = new QTabWidget(this->uiForm->tabWidget);
    OneDataTableViewer* oneDataTableViewer = new OneDataTableViewer();
    daoViewer->insertOneLeaf(tempWidgetTable,oneDataTableViewer);

    tempWidgetTable->setGeometry(0,
                                 0,
                                 this->uiForm->tabWidget->size().width(),
                                 this->uiForm->tabWidget->size().height());

    this->uiForm->tabWidget->addTab(tempWidgetTable,"DataBase Viewer");

    tempWidgetTable->setTabsClosable(true);
    tempWidgetTable->setTabPosition(QTabWidget::South);
    tempWidgetTable->setDocumentMode(true);

    const std::list<QString> & tables = DAO::getInstance()->getAllTablesName();
    for(std::list<QString>::const_reverse_iterator it = tables.rbegin();
                                                   it != tables.rend();
                                                   ++it){

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
        compents->model->setQuery(QString("select * from %1").arg(*it));

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

void TRTimeOperator::storageSetting(){
    QMessageBox::information(nullptr,"Info","123");
}

void TRTimeOperator::patientInfoSetting(){
    PatientInfoSetting *form = new PatientInfoSetting(this);
    form->exec();

    delete form;
    form = NULL;
}

void TRTimeOperator::pipleLineSetting(){
    QMessageBox::information(nullptr,"Info","789");
}

bool TRTimeOperator::timeRecord(unsigned int buttonID){
    bool result = false;
    QString time = QTime::currentTime().toString("hh:mm:ss");
    QString tableName = "Date_";

    if(0 == buttonID){
        this->inputPatientInfo(PatientInputMode(ViewAndModify));
    }

    if(this->buttonTimeRecord.find(buttonID) == this->buttonTimeRecord.end()){
        this->buttonTimeRecord.insert(std::pair<unsigned int,QString>(buttonID,time));
    }else{
        QMessageBox::critical(nullptr, "Error", QString("Button Index %1 is repeated").arg(buttonID));
        exit(-1);
    }

    if((unsigned int)(buttonID+1) == ConfigLoader::getInstance()->getTheOperatorPatten()->size()){
        tableName.append(QDate::currentDate().toString("yyyy_MM_dd"));

        DAO::getInstance()->updateTableName(tableName,
                                            *ConfigLoader::getInstance()->getThePatientInfoPatten(),
                                            *ConfigLoader::getInstance()->getTheOperatorPatten());

        this->lastTableName = tableName;

        DAO::getInstance()->appendARow(tableName,patientInfoRecord,buttonTimeRecord);
        CSVWriter::getInstance()->appendARecord(tableName,patientInfoRecord,buttonTimeRecord);

        std::map<unsigned int,QString>().swap(this->patientInfoRecord);
        this->patientInfoRecord.clear();

        std::map<unsigned int,QString>().swap(this->buttonTimeRecord);
        this->buttonTimeRecord.clear();

        result = true;
    }

    return result;
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
            DAO::getInstance()->deleteLastRecord(this->lastTableName);
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
    PatientInput * patientForm = new PatientInput(this,&this->patientInfoRecord,model);
    patientForm->exec();
    const std::map<unsigned int,QString>* info = patientForm->getInfos();

    for(std::map<unsigned int,QString>::const_iterator it = info->begin();
                                                       it != info->end();
                                                       it++){
        this->patientInfoRecord.insert(*it);
    }

    delete patientForm;
    patientForm = NULL;
}
