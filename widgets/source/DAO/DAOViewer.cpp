#include "../../include/DAO/DAOViewer.h"

OneDataTableViewer::OneDataTableViewer(){
    this->clear();
}

OneDataTableViewer::~OneDataTableViewer(){
    this->clear();
}

OneDataTableViewer::OneDataTableViewer(const OneDataTableViewer & subject){
    //shadow copy
    *this = subject;
}

const OneDataTableViewer & OneDataTableViewer::operator = (const OneDataTableViewer & subject){
    //shadow copy
    this->mainWidget = subject.mainWidget;
    this->tableView = subject.tableView;
    this->model = subject.model;
    this->insertButton = subject.insertButton;
    this->deleteButton = subject.deleteButton;

    return *this;
}


void OneDataTableViewer::clear(){
    if(this->mainWidget){
        this->mainWidget->close();
        delete this->mainWidget;
        this->mainWidget = NULL;
    }

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

/*DAOViewer*/
DAOViewer::DAOViewer(){
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


void DAOViewer::clear(){
    if(this->viewTree){
        for(std::map<QTabWidget*,OneDataTableViewer*>::iterator it = this->viewTree->begin();
                                                                      it != this->viewTree->end();
                                                                      it++){
            it->first->close();
            delete it->first;

            delete it->second;
            it->second = NULL;
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
