#include "../../../include/Storage/DAO/Storage_DAO_Viewer.h"
#include "qsqlerror.h"
#include <QSqlRecord>
#include <QMessageBox>
#include <QString>
#include <QInputDialog>
#include <QLineEdit>

OneDataTableViewerCompents::OneDataTableViewerCompents(){
    this->clear();
}

OneDataTableViewerCompents::~OneDataTableViewerCompents(){
    this->clear();
}

OneDataTableViewerCompents::OneDataTableViewerCompents(const OneDataTableViewerCompents & subject){
    //shadow copy
    *this = subject;
}

const OneDataTableViewerCompents & OneDataTableViewerCompents::operator = (const OneDataTableViewerCompents & subject){
    //shadow copy
    this->tableView = subject.tableView;
    this->model = subject.model;
    this->insertButton = subject.insertButton;
    this->deleteButton = subject.deleteButton;

    return *this;
}


void OneDataTableViewerCompents::clear(){
    if(this->tableView){
        this->tableView->close();
        delete this->tableView;
        this->tableView = NULL;
    }

    if(this->model){
        delete this->model;
        this->model = NULL;
    }

    if(this->insertButton){
        delete this->insertButton;
        this->insertButton = NULL;
    }

    if(this->deleteButton){
        delete this->deleteButton;
        this->deleteButton = NULL;
    }
}

/*OneDataTableViewer*/
OneDataTableViewer::OneDataTableViewer(){
    this->clear();
}

OneDataTableViewer::~OneDataTableViewer(){
    this->clear();
}

OneDataTableViewer::OneDataTableViewer(const OneDataTableViewer & subject){
    *this = subject;
}

const OneDataTableViewer & OneDataTableViewer::operator=(const OneDataTableViewer & subject){
    this->clear();

    for(std::map<QWidget*,OneDataTableViewerCompents*>::const_iterator it = subject.oneViewer.begin();
                                                                       it != subject.oneViewer.end();
                                                                       it++){
        this->oneViewer.insert(*it);
    }

    return *this;
}

void OneDataTableViewer::insertOneLeaf(QWidget* widget,OneDataTableViewerCompents* compents){
    if(this->oneViewer.count(widget) > 0){
        QMessageBox::critical(nullptr,"Error",QString("The dataView table %1 is duplicated!").arg(reinterpret_cast<const char*>(widget)));
        exit(-1);
    }

    this->oneViewer.insert(std::pair<QWidget*,OneDataTableViewerCompents*>(widget,compents));
}

void OneDataTableViewer::clear(){
    for(std::map<QWidget*,OneDataTableViewerCompents*>::iterator it = this->oneViewer.begin();
                                                                 it != this->oneViewer.end();
                                                                 it++){
        if(it->second){
            delete it->second;
            it->second = NULL;
        }

        if(it->first){
            it->first->close();
            delete it->first;
        }
    }

    std::map<QWidget*,OneDataTableViewerCompents*>().swap(this->oneViewer);
    this->oneViewer.clear();
}

/*DAOViewer*/
DAOViewer::DAOViewer(QObject* parent):QObject(parent){
    this->clear();
}

DAOViewer::~DAOViewer(){
    this->clear();
}

DAOViewer::DAOViewer(const DAOViewer & subject):QObject(NULL){
    *this = subject;
}


const DAOViewer&  DAOViewer::operator = (const DAOViewer & subject){
    /*shadow copy*/
    this->clear();

    if(subject.viewTree){
        this->viewTree = new std::map<QTabWidget*,OneDataTableViewer*>();
        for(std::map<QTabWidget*,OneDataTableViewer*>::const_iterator it = subject.viewTree->begin();
                                                                      it != subject.viewTree->end();
                                                                      it++){
            this->viewTree->insert(*it);
        }
    }

    if(subject.map_Button_SqlTableModel){
        this->map_Button_SqlTableModel = new std::map<QObject*,QSqlTableModel*>();
        for(std::map<QObject*,QSqlTableModel*>::const_iterator it = subject.map_Button_SqlTableModel->begin();
                                                               it != subject.map_Button_SqlTableModel->end();
                                                               it++){
            this->map_Button_SqlTableModel->insert(*it);
        }
    }

    if(subject.map_Button_TableView){
        this->map_Button_TableView = new std::map<QObject*,QTableView*>();
        for(std::map<QObject*,QTableView*>::const_iterator it = subject.map_Button_TableView->begin();
                                                           it != subject.map_Button_TableView->end();
                                                           it++){
            this->map_Button_TableView->insert(*it);
        }
    }

    return *this;
}


void DAOViewer::insertOneLeaf(QTabWidget* widget,OneDataTableViewer* oneView){
    if(!this->viewTree){
        this->viewTree = new std::map<QTabWidget*,OneDataTableViewer*>();
    }

    if(this->viewTree->count(widget) > 0){
        QMessageBox::critical(nullptr,"Error",QString("The dataView bar %1 is duplicated!").arg(reinterpret_cast<const char*>(widget)));
        exit(-1);
    }

    this->viewTree->insert(std::pair<QTabWidget*,OneDataTableViewer*>(widget,oneView));
}

void DAOViewer::deleteOneLeaf(QTabWidget* widget){
    if(this->viewTree){
        auto it = this->viewTree->find(widget);
        if(this->viewTree->end() != it){
            delete it->second;
            it->second = NULL;
            delete it->first;

            this->viewTree->erase(widget);
        }
    }
}

void DAOViewer::bindOneButtonToSqlModel(QObject* button,QSqlTableModel* model){
    if(!this->map_Button_SqlTableModel){
        this->map_Button_SqlTableModel = new std::map<QObject*,QSqlTableModel*>();
    }

    if(this->map_Button_SqlTableModel->count(button) > 0){
        QMessageBox::critical(nullptr,"Error",QString("The button %1 is already binded!").arg(reinterpret_cast<const char*>(button)));
        exit(-1);
    }

    this->map_Button_SqlTableModel->insert(std::pair<QObject*,QSqlTableModel*>(button,model));
}

void DAOViewer::bindOneButtonToTableView(QObject* button,QTableView* table){
    if(!this->map_Button_TableView){
        this->map_Button_TableView = new std::map<QObject*,QTableView*>();
    }

    if(this->map_Button_TableView->count(button) > 0){
        QMessageBox::critical(nullptr,"Error",QString("The button %1 is already binded!").arg(reinterpret_cast<const char*>(button)));
        exit(-1);
    }

    this->map_Button_TableView->insert(std::pair<QObject*,QTableView*>(button,table));
}

void DAOViewer::clear(){
    if(this->viewTree){
        for(std::map<QTabWidget*,OneDataTableViewer*>::iterator it = this->viewTree->begin();
                                                                      it != this->viewTree->end();
                                                                      it++){
            if(it->first){
                it->first->close();
                delete it->first;
            }

            if(it->second){
                delete it->second;
                it->second = NULL;
            }
        }

        std::map<QTabWidget*,OneDataTableViewer*>().swap(*this->viewTree);
        this->viewTree->clear();
        delete this->viewTree;
        this->viewTree = NULL;
    }

    if(this->map_Button_SqlTableModel){
        std::map<QObject*,QSqlTableModel*>().swap(*this->map_Button_SqlTableModel);
        this->map_Button_SqlTableModel->clear();
        delete this->map_Button_SqlTableModel;
        this->map_Button_SqlTableModel = NULL;
    }

    if(this->map_Button_TableView){
        std::map<QObject*,QTableView*>().swap(*this->map_Button_TableView);
        this->map_Button_TableView->clear();
        delete this->map_Button_TableView;
        this->map_Button_TableView = NULL;
    }
}

void DAOViewer::deleteRow(QObject* obj){
    QString inputed;
    bool OK = true;
    QSqlTableModel::EditStrategy strategy;
    QModelIndexList list;
    QTableView * view = NULL;
    QSqlTableModel *model = NULL;

    if(this->map_Button_SqlTableModel){
        std::map<QObject*,QSqlTableModel*>::iterator it = this->map_Button_SqlTableModel->find(obj);
        if( this->map_Button_SqlTableModel->end() == it){
            QMessageBox::critical(nullptr,"Error",QString("The button %1 is not binded!").arg(reinterpret_cast<const char*>(obj)));
            exit(-1);
        }

        if(it->second){
            model = it->second;
        }else{
            QMessageBox::critical(nullptr,"Error",QString("The button %1 is not binded to valid data model!").arg(reinterpret_cast<const char*>(obj)));
            exit(-1);
        }
    }else{
        QMessageBox::critical(nullptr,"Error","None button is binded");
    }

    if(this->map_Button_TableView){
        std::map<QObject*,QTableView*>::iterator it = this->map_Button_TableView->find(obj);
        if( this->map_Button_TableView->end() == it){
            QMessageBox::critical(nullptr,"Error",QString("The button %1 is not binded!").arg(reinterpret_cast<const char*>(obj)));
            exit(-1);
        }

        if(it->second){
            view = it->second;
        }else{
            QMessageBox::critical(nullptr,"Error",QString("The button %1 is not binded to valid view!").arg(reinterpret_cast<const char*>(obj)));
            exit(-1);
        }
    }else{
        QMessageBox::critical(nullptr,"Error","None button is binded");
    }

    if(!(inputed = QInputDialog::getText(view,QString("Delete confirm"),QString("Please input 'hfimc'"),QLineEdit::Password,0,&OK)).isEmpty() && OK && (inputed != QString("hfimc"))){
        QMessageBox::information(nullptr,"Error",QString("Input: %1, you should input 'hficm'").arg(inputed));
    }
    if(OK && inputed == QString("hfimc")){
        strategy =  model->editStrategy();
        model->setEditStrategy(QSqlTableModel::OnManualSubmit);

        list = view->selectionModel()->selectedRows();

        for(unsigned int i = 0;i<list.size();i++){
            model->removeRow(list.at(i).row());
        }
        model->submitAll();
        model->select();

        model->setEditStrategy(strategy);
    }
}

void DAOViewer::appendARow(QObject* obj){
    QString inputed;
    bool OK = true;
    QSqlTableModel::EditStrategy strategy;
    if(this->map_Button_SqlTableModel){
        std::map<QObject*,QSqlTableModel*>::iterator it = this->map_Button_SqlTableModel->find(obj);
        if( this->map_Button_SqlTableModel->end() == it){
            QMessageBox::critical(nullptr,"Error",QString("The button %1 is not binded!").arg(reinterpret_cast<const char*>(obj)));
            exit(-1);
        }

        if(it->second){
            if(!(inputed = QInputDialog::getText(nullptr,QString("insert confirm"),QString("Please input 'hfimc'"),QLineEdit::Password,0,&OK)).isEmpty() && OK && (inputed != QString("hfimc"))){
                QMessageBox::information(nullptr,"Error",QString("Input: %1, you should input 'hficm'").arg(inputed));
            }
            if(OK && inputed == QString("hfimc")){

                strategy = it->second->editStrategy();
                it->second->setEditStrategy(QSqlTableModel::OnManualSubmit);
                int count = it->second->rowCount();
                it->second->insertRow(count);
                it->second->setData(it->second->index(count,0),count);
                if(!it->second->submitAll()){
                    QMessageBox::critical(nullptr,"Error",it->second->lastError().text());
                }
                //it->second->select();
                it->second->setEditStrategy(strategy);
            }
        }else{
            QMessageBox::critical(nullptr,"Error",QString("The button %1 is not binded to valid data model!").arg(reinterpret_cast<const char*>(obj)));
            exit(-1);
        }
    }else{
        QMessageBox::critical(nullptr,"Error","None button is binded");
    }
}
