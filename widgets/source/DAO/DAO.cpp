#include "../../include/DAO/DAO.h"
#include "../../include/Config/ConfigLoader.h"
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
        this->theDataBase->close();
        this->theDataBase->removeDatabase(systemDBPath);
        delete this->theDataBase;
        this->theDataBase = NULL;
    }
}

/*Realize the interface*/
bool DAO::tableExisted(const QString & tableName){
    bool result = false;
    QSqlQuery query;
    query.exec(QString("select tbl_name from sqlite_master WHERE type = 'table' AND tbl_name = '%1';").arg(tableName));
    qDebug()<<query.lastError();
    if(query.next()){
        if(query.value(0).isValid() && query.value(0).toString() == tableName){
            result = true;
        }
    }

    return result;
}

std::list<QString> DAO::getAllTablesName(){
    std::list<QString> result;

    QSqlQuery query;
    query.exec(QString("select tbl_name from sqlite_master WHERE type = 'table';"));
    qDebug()<<query.lastError();
    while(query.next()){
        result.push_back(query.value(0).toString());
    }

    return result;
}

QString DAO::getRowCount(const QString & tableName){
    QString result("0");
    QSqlQuery query;
    query.exec(QString("select count (*) from %1 ;").arg(tableName));
    qDebug()<<query.lastError();
    if(query.next()){
        if(query.value(0).isValid()){
            result = query.value(0).toString();
        }
    }
    return result;
}

void DAO::createEmptyTable(const QString & tableName){
    QSqlQuery query;
    QString str("create table ");
    str.append(tableName).append("(id int primary key");

    const std::map<unsigned int,QString> * patientInfoPatten = ConfigLoader::getInstance()->getThePatientInfoPatten();
    for(std::map<unsigned int,QString>::const_iterator it = patientInfoPatten->begin();
        it != patientInfoPatten->end();
        it++){
        str.append(", ").append(it->second).append(" varchar(50)");
    }

    const std::map<unsigned int,QString> * operatorPatten = ConfigLoader::getInstance()->getTheOperatorPatten();
    for(std::map<unsigned int,QString>::const_iterator it = operatorPatten->begin(); it != operatorPatten->end(); it++){
        str.append(", ").append(it->second).append("Time varchar(20)");
    }
    str.append(");");

    query.exec(str);
    qDebug()<<query.lastError();
}

void DAO::appendARow(const QString & tableName,
                     const std::map<unsigned int,QString> & patientInfos,
                     const std::map<unsigned int,QString> & operatorTimes){
    QString count;
    if(!DAO::getInstance()->tableExisted(tableName)){
        qDebug()<<"Table is not existed, create a new table: "<<tableName;
        DAO::getInstance()->createEmptyTable(tableName);
        count = "0";
    }else{
        count = getRowCount(tableName);
    }

    QSqlQuery query;
    QString str("INSERT INTO ");
    str.append(tableName).append(" (id");
    const std::map<unsigned int,QString> * patientInfoPatten = ConfigLoader::getInstance()->getThePatientInfoPatten();
    for(std::map<unsigned int,QString>::const_iterator it = patientInfoPatten->begin(); it != patientInfoPatten->end(); it++){
        str.append(", ").append(it->second);
    }

    const std::map<unsigned int,QString> * operatorPatten = ConfigLoader::getInstance()->getTheOperatorPatten();
    for(std::map<unsigned int,QString>::const_iterator it = operatorPatten->begin(); it != operatorPatten->end(); it++){
        str.append(", ").append(it->second).append("Time");
    }

    str.append(") VALUES (").append(count);

    for(auto it = patientInfoPatten->begin(); it != patientInfoPatten->end();it++){
        auto it_find = patientInfos.find(it->first);
        if(it_find == patientInfos.end()){
            str.append(", '").append("").append("'");
        }else{
            str.append(", '").append(it_find->second).append("'");
        }
    }

    for(auto it = operatorPatten->begin(); it != operatorPatten->end();it++){
        auto it_find = operatorTimes.find(it->first);
        if(it_find == operatorTimes.end()){
            QMessageBox::information(nullptr, "Warning", QString("Operator %1 time is not found").arg(it->second));
            str.append(", '").append("").append("'");
        }else{
            str.append(", '").append(it_find->second).append("'");
        }
    }

    str.append(");");

    query.exec(str);
    if(QSqlError::NoError != query.lastError().type()){
        QMessageBox::critical(nullptr,"Error",query.lastError().text());
        exit(-1);
    }
}

void DAO::deleteLastRecord(const QString & tableName){
    QString str("");
    if(DAO::getInstance()->tableExisted(tableName)){
        QSqlQuery query;
        str = str.append("delete from %1 where id like ("
                                                     "select id from %1 order by id desc limit 1"
                                                     ")"
                         ";").arg(tableName);
        query.exec(str);

        if(QSqlError::NoError != query.lastError().type()){
            QMessageBox::critical(nullptr,"Error",query.lastError().text());
            exit(-1);
        }
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
