#include "../include/DAO.h"
#include "../include/ConfigLoader.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>

static QString systemDBPath = "TR.db";
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

/*Realize the interface*/
bool DAO::tableExisted(const QString & tableName){
    QSqlQuery query;
    query.exec(QString("select  %1 from sqlite_master").arg(tableName));
    return query.next();
}

QString DAO::getRowCount(const QString & tableName){
    QSqlQuery query;
    query.exec(QString("select count (*) as nums from %1").arg(tableName));
    return query.value("nums").toString();
}

void DAO::createEmptyTable(const QString & tableName){
    QSqlQuery query;
    QString str("create table ");
    str.append(tableName).append("(id int primary key");
    const std::map<int,QString> * operatorPatten = ConfigLoader::getInstance()->getTheOperatorPatten();
    for(std::map<int,QString>::const_iterator it = operatorPatten->begin(); it != operatorPatten->end(); it++){
        str.append(", ").append(it->second).append("Time varchar(20)");
    }
    str.append(")");

    query.exec(str);
}

void DAO::appendARow(const QString & tableName,const std::map<int,QString> & operatorTimes){
    QString count = getRowCount(tableName);
    QSqlQuery query;
    QString str("INSERT INTO ");
    str.append(tableName).append(" (id");
    const std::map<int,QString> * operatorPatten = ConfigLoader::getInstance()->getTheOperatorPatten();
    for(std::map<int,QString>::const_iterator it = operatorPatten->begin(); it != operatorPatten->end(); it++){
        str.append(", ").append(it->second).append("Time");
    }
    str.append(") VALUES (").append(count);
    for(uint i = 0; i < operatorPatten->size();i++){
        str.append(", ").append(operatorTimes.at(i));
    }

    str.append(")");

    query.exec(str);
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
