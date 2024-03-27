#include "../../../include/Storage/CSV/Storage_CSV_Viewer.h"
#include <QMessageBox>

OneCSVViewerCompents::OneCSVViewerCompents(){
    this->clear();
}

OneCSVViewerCompents::~OneCSVViewerCompents(){
    this->clear();
}

OneCSVViewerCompents::OneCSVViewerCompents(const OneCSVViewerCompents & subject){
    //shadow copy
    *this = subject;
}

const OneCSVViewerCompents & OneCSVViewerCompents::operator = (const OneCSVViewerCompents & subject){
    //shadow copy
    this->tableView = subject.tableView;

    return *this;
}


void OneCSVViewerCompents::clear(){
    if(this->tableView){
        this->tableView->close();
        delete this->tableView;
        this->tableView = NULL;
    }
}

/*OneCSVViewer*/
OneCSVViewer::OneCSVViewer(){
    this->clear();
}

OneCSVViewer::~OneCSVViewer(){
    this->clear();
}

OneCSVViewer::OneCSVViewer(const OneCSVViewer & subject){
    *this = subject;
}

const OneCSVViewer & OneCSVViewer::operator=(const OneCSVViewer & subject){
    this->clear();

    for(std::map<QWidget*,OneCSVViewerCompents*>::const_iterator it = subject.oneViewer.begin();
         it != subject.oneViewer.end();
         it++){
        this->oneViewer.insert(*it);
    }

    return *this;
}

void OneCSVViewer::insertOneLeaf(QWidget* widget,OneCSVViewerCompents* compents){
    if(this->oneViewer.count(widget) > 0){
        QMessageBox::critical(nullptr,"Error",QString("The dataView table %1 is duplicated!").arg(reinterpret_cast<const char*>(widget)));
        exit(-1);
    }

    this->oneViewer.insert(std::pair<QWidget*,OneCSVViewerCompents*>(widget,compents));
}

void OneCSVViewer::clear(){
    for(std::map<QWidget*,OneCSVViewerCompents*>::iterator it = this->oneViewer.begin();
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
}

/*CSVViewer*/
CSVViewer::CSVViewer(QObject* parent):QObject(parent){
    this->clear();
}

CSVViewer::~CSVViewer(){
    this->clear();
}

CSVViewer::CSVViewer(const CSVViewer & subject):QObject(NULL){
    *this = subject;
}


const CSVViewer&  CSVViewer::operator = (const CSVViewer & subject){
    /*shadow copy*/
    this->clear();

    if(subject.viewTree){
        this->viewTree = new std::map<QTabWidget*,OneCSVViewer*>();
        for(std::map<QTabWidget*,OneCSVViewer*>::const_iterator it = subject.viewTree->begin();
             it != subject.viewTree->end();
             it++){
            this->viewTree->insert(*it);
        }
    }

    return *this;
}


void CSVViewer::insertOneLeaf(QTabWidget* widget,OneCSVViewer* oneView){
    if(!this->viewTree){
        this->viewTree = new std::map<QTabWidget*,OneCSVViewer*>();
    }

    if(this->viewTree->count(widget) > 0){
        QMessageBox::critical(nullptr,"Error",QString("The dataView bar %1 is duplicated!").arg(reinterpret_cast<const char*>(widget)));
        exit(-1);
    }

    this->viewTree->insert(std::pair<QTabWidget*,OneCSVViewer*>(widget,oneView));
}

void CSVViewer::deleteOneLeaf(QTabWidget* widget){
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

void CSVViewer::clear(){
    if(this->viewTree){
        for(std::map<QTabWidget*,OneCSVViewer*>::iterator it = this->viewTree->begin();
             it != this->viewTree->end();
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

        std::map<QTabWidget*,OneCSVViewer*>().swap(*this->viewTree);
        this->viewTree->clear();
        delete this->viewTree;
        this->viewTree = NULL;
    }
}
