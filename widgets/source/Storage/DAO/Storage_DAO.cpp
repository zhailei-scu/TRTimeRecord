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
    this->clear();
    /*tr record in local*/
    this->trInfoConnection = new DAO_Sqlite(NULL,"trInfoConnection");

    /*patient info in online or local*/
    this->patientInfoReConnect();
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

DAO_Interface * DAO::getPatientInfoConnection(){
    return thePtr->patientInfoConnection;
}

DAO_Interface * DAO::getTrInfoConnection(){
    return thePtr->trInfoConnection;
}

void DAO::patientInfoReConnect(){
    int result = 0;
    if(this->patientInfoConnection){
        delete this->patientInfoConnection;
        this->patientInfoConnection = NULL;
    }
    //Test mysql online connection
    OnlineDatabaseSetting* databaseSettingForm = NULL;
    DAO_Mysql* mysqlConnection = new DAO_Mysql(NULL,"patientInfoReConnect");
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
            DAO_Sqlite* sqliteConnection = new DAO_Sqlite(NULL,"patientInfoReConnect");
            if(!sqliteConnection->isDataBaseOpened()){
                QMessageBox::information(nullptr, "Error","Open sqlite error");
                exit(-1);
            }
            this->patientInfoConnection = sqliteConnection;
        }else{
            databaseSettingForm = new OnlineDatabaseSetting(NULL);
            result = databaseSettingForm->exec();

            delete databaseSettingForm;
            databaseSettingForm = NULL;

            if(QDialog::Accepted == result){
                patientInfoReConnect();
            }
        }
    }else{
        this->patientInfoConnection = mysqlConnection;
    }
}

void DAO::Start(){
    if(!thePtr){
        thePtr = new DAO();
    }

    if(DAO::getInstance()->getPatientInfoConnection()->needToUpdateTable_Patient(*ConfigLoader::getInstance()->getThePatientInfoPatten())){
        DAO::getInstance()->getPatientInfoConnection()->updateTable_Patient();
    }

    qDebug()<<"DataBase Started...";
}

void DAO::clear(){
    if(this->patientInfoConnection){
        delete this->patientInfoConnection;
        this->patientInfoConnection = NULL;
    }

    if(this->trInfoConnection){
        delete this->trInfoConnection;
        this->trInfoConnection = NULL;
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
