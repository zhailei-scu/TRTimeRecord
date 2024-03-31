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
    this->clear();
    //Test mysql online connection
    DAO_Mysql* mysqlConnection = new DAO_Mysql();
    if(!mysqlConnection->isDataBaseOpened()){
        delete mysqlConnection;
        mysqlConnection = NULL;

        QMessageBox::information(nullptr,"Information","The online data base connection is invalid, a local data base is applied");

        DAO_Sqlite* sqliteConnection = new DAO_Sqlite();

        if(!sqliteConnection->isDataBaseOpened()){
            QMessageBox::information(nullptr, "Error","Open sqlite error");
            exit(-1);
        }
        this->connection = sqliteConnection;
    }else{
        this->connection = mysqlConnection;
    }
}

DAO::~DAO(){
    this->clear();
}

DAO_Interface * DAO::getConnection(){
    if(!thePtr){
        //Test mysql online connection
        thePtr = new DAO();
    }
    return thePtr->connection;
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
