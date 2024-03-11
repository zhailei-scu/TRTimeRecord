#include "../include/DAO.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>

static QString systemDBPath = "E:\\TRTimeRecord\\dataBase\\TR.db";
static QString systemCSVPath = "E:\\TRTimeRecord\\dataBase\\TR.csv";

/*
 * In current softwares, there are two place to record the operation data:
 * (1) The .csv file recorded the patient therapy time, located in user specialed path
 * (2) An opensource lightting database-SQLite database recorded all operations data, which located in default path.
*/

/* May change to MS access
 * static QString dataBaseStr(QString("DRIVER={Microsoft Access Driver (*.mdb, *.accdb)};FIL={TR};DBQ=%1").arg(dbmFilePath));
 * QSqlDatabase sq = QSqlDatabase::addDatabase("QODBC");
 * sq.setDatabaseName(dataBaseStr);
 * but always error
*/

DAO* DAO::thePtr = nullptr;
DAO::GbClear DAO::m_GbClear;

DAO::DAO(){
    this->clear();
    this->theDataBase = new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE"));
    this->theDataBase->setDatabaseName(systemDBPath);
    if(!this->theDataBase->open()){
        QMessageBox::information(nullptr, "Error",this->theDataBase->lastError().text());
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
    qDebug()<<"DataBase Started...";
}

void DAO::clear(){
    if(this->theDataBase){
        this->theDataBase->removeDatabase(systemDBPath);
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
