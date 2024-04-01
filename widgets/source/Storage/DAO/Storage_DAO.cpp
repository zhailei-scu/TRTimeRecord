#include "../../../include/Storage/DAO/Storage_DAO.h"
#include "../../../include/Storage/DAO/Storage_DAO_Sqlite.h"
#include "../../../include/Storage/DAO/Storage_DAO_Mysql.h"
#include "../../../include/Form/OnlineDatabaseSetting/Form_OnlineDatabaseSetting.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QApplication>
#include <QScreen>
#include <QPalette>

DAO* DAO::thePtr = nullptr;
DAO::GbClear DAO::m_GbClear;

DAO::DAO(){
    this->reConnect();
}

DAO::~DAO(){
    this->clear();
}

DAO * DAO::getInstance(){
    if(!thePtr){
        QMessageBox::critical(nullptr,"Error","You must Start the DAO before!");
        exit(-1);
    }
    return thePtr;
}

DAO_Interface * DAO::getConnection(){
    return thePtr->connection;
}

void DAO::reConnect(){
    int result = 0;
    this->clear();
    //Test mysql online connection
    OnlineDatabaseSetting* databaseSettingForm = NULL;
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
            databaseSettingForm = new OnlineDatabaseSetting(NULL);
            result = databaseSettingForm->exec();

            delete databaseSettingForm;
            databaseSettingForm = NULL;

            if(QDialog::Accepted == result){
                reConnect();
            }
        }
    }else{
        this->connection = mysqlConnection;
    }
}

void DAO::Start(){
    if(!thePtr){
        thePtr = new DAO();
    }
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
