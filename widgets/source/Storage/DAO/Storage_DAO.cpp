#include "../../../include/Storage/DAO/Storage_DAO.h"
#include "../../../include/Storage/DAO/Storage_DAO_Sqlite.h"
#include "../../../include/Storage/DAO/Storage_DAO_Mysql.h"
#include "../../../include/Global/Config/Global_Config_ConfigLoader.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <sstream>
#include <QSqlError>
#include <QMessageBox>

DAO* DAO::thePtr = nullptr;
DAO::GbClear DAO::m_GbClear;

DAO::DAO(){
    this->reConnect();
}

DAO::~DAO(){
    this->clear();
}

DAO_Interface * DAO::getConnection(){
    if(!thePtr){
        thePtr = new DAO();
    }
    return thePtr->connection;
}

void DAO::reConnect(){
    int result = 0;
    this->clear();
    //Test mysql online connection
    DAO_Mysql* mysqlConnection = new DAO_Mysql();
    if(!mysqlConnection->isDataBaseOpened()){
        delete mysqlConnection;
        mysqlConnection = NULL;

        result = QMessageBox::information(nullptr,
                                          "Information",
                                          "The online database is not valid,"
                                          "click 'Yes' button to re-configure online database,"
                                          "click 'No' button to just use local database?",
                                          QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

        if(QMessageBox::Yes != result){
            DAO_Sqlite* sqliteConnection = new DAO_Sqlite();
            if(!sqliteConnection->isDataBaseOpened()){
                QMessageBox::information(nullptr, "Error","Open sqlite error");
                exit(-1);
            }
            this->connection = sqliteConnection;
        }else{
            reConnect();
        }
    }else{
        this->connection = mysqlConnection;
    }
}

void DAO::Start(){
    getConnection();
    qDebug()<<"DataBase Started...";
}

void DAO::clear(){
    if(this->connection){
        delete this->connection;
        this->connection = NULL;
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
