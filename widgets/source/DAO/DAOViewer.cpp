#include "../../include/DAO/DAOViewer.h"
#include <QMessageBox>
#include <QString>

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
        if(it->first){
            it->first->close();
            delete it->first;
        }

        if(it->second){
            delete it->second;
            it->second = NULL;
        }
    }
}

/*DAOViewer*/
DAOViewer::DAOViewer(QObject* parent):QObject(parent){
    this->clear();
}

DAOViewer::~DAOViewer(){
    this->clear();
}

DAOViewer::DAOViewer(const DAOViewer & subject){
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

    if(subject.map_InsertButton_SqlTableModel){
        this->map_InsertButton_SqlTableModel = new std::map<QObject*,QSqlTableModel*>();
        for(std::map<QObject*,QSqlTableModel*>::const_iterator it = subject.map_InsertButton_SqlTableModel->begin();
                                                               it != subject.map_InsertButton_SqlTableModel->end();
                                                               it++){
            this->map_InsertButton_SqlTableModel->insert(*it);
        }
    }

    if(subject.map_DeletetButton_SqlTableModel){
        this->map_DeletetButton_SqlTableModel = new std::map<QObject*,QSqlTableModel*>();
        for(std::map<QObject*,QSqlTableModel*>::const_iterator it = subject.map_DeletetButton_SqlTableModel->begin();
                                                               it != subject.map_DeletetButton_SqlTableModel->end();
                                                               it++){
            this->map_DeletetButton_SqlTableModel->insert(*it);
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

    if(this->map_InsertButton_SqlTableModel){
        std::map<QObject*,QSqlTableModel*>().swap(*this->map_InsertButton_SqlTableModel);
        this->map_InsertButton_SqlTableModel->clear();
        delete this->map_InsertButton_SqlTableModel;
        this->map_InsertButton_SqlTableModel = NULL;
    }

    if(this->map_DeletetButton_SqlTableModel){
        std::map<QObject*,QSqlTableModel*>().swap(*this->map_DeletetButton_SqlTableModel);
        this->map_DeletetButton_SqlTableModel->clear();
        delete this->map_DeletetButton_SqlTableModel;
        this->map_DeletetButton_SqlTableModel = NULL;
    }
}

void DAOViewer::deleteRow(QObject* obj){
    QMessageBox::information(nullptr,"Info",obj->objectName());
    //table->insertRecord(table->rowCount(),table->record());
}

void DAOViewer::appendARow(QObject* obj){
    QMessageBox::information(nullptr,"Info",obj->objectName());
    //table->insertRecord(table->rowCount(),table->record());
}
