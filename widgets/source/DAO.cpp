#include "../include/DAO.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>

static QString dataBaseStr(QString("DRIVER={Microsoft Access Driver (*.mdb, *accdb)};FIL={MS Access};DBQ=%1;Uid=%2,Pwd=%3")
                               .arg("D:\\Development\\TRTime\\dataBase\\TRTimeRecord.accdb")
                               .arg("hfcim")
                               .arg("hfcim"));

DAO* DAO::thePtr = nullptr;
DAO::GbClear DAO::m_GbClear;

DAO::DAO(){
    this->clear();
    /*
    this->theDataBase = new QSqlDatabase();
    this->theDataBase->addDatabase("QODBC");
    this->theDataBase->setDatabaseName(dataBaseStr);
*/

    QSqlDatabase sq = QSqlDatabase::addDatabase("QODBC");
    sq.setDatabaseName(dataBaseStr);

    /*
    if(!this->theDataBase->isOpen()){
        QMessageBox::information(nullptr, "Error",this->theDataBase->lastError().text());
    }
    */

    if(!sq.isOpen()){
        QMessageBox::information(nullptr, "Error",sq.lastError().text());
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
