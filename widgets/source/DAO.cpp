#include "../include/DAO.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>

static QString dataBaseStr(QString("DRIVER={Microsoft Access Driver (*.mdb, *.accdb)};FIL={MS Access};DBQ=%1;Uid=%2;Pwd=%3")
                               .arg("D:\Development\TRTime\dataBase\TRTimeRecord.accdb")
                               .arg("hfcim")
                               .arg("hfcim"));

DAO* DAO::thePtr = nullptr;
DAO::GbClear DAO::m_GbClear;

DAO::DAO(){
    this->clear();
    this->dataBase = new QSqlDatabase();
    this->dataBase->addDatabase("QODBC","accessDB");

    this->dataBase->setDatabaseName(dataBaseStr);

    if(!this->dataBase->isOpen()){
        QMessageBox::information(nullptr, "Error", "DataBase connect error");
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
