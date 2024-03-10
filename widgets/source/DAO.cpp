#include "../include/DAO.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>

static QString dataBaseStr(QString("DRIVER={Microsoft Access Driver (*.mdb)};FIL={MS Access};DBQ=%1")
                               .arg("D:\\Development\\TRTime\\dataBase\\TRTimeRecord.mdb"));

DAO* DAO::thePtr = nullptr;
DAO::GbClear DAO::m_GbClear;

DAO::DAO(){
    this->clear();
    this->theDataBase = new QSqlDatabase(QSqlDatabase::addDatabase("QODBC"));

    this->theDataBase->setDatabaseName(dataBaseStr);

    if(!this->theDataBase->isOpen()){
        QMessageBox::information(nullptr, "Error",this->theDataBase->lastError().text());

    }else{
        QMessageBox::information(nullptr, "Success", "DataBase connect success");
    }
}

DAO::~DAO(){
    this->clear();
}

DAO * DAO::getInstance(){
    if(!thePtr){
        thePtr = new DAO();
    }
    return thePtr;
}

void DAO::Start(){
    getInstance();
}

void DAO::clear(){
    if(this->theDataBase){
        this->theDataBase->removeDatabase(dataBaseStr);
        delete this->theDataBase;
        this->theDataBase = NULL;
    }
}

/*Garbge clear*/
DAO::GbClear::GbClear(){

}

DAO::GbClear::~GbClear(){
    if(thePtr){
        delete thePtr;
        thePtr = NULL;
    }
}
