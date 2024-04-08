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
    this->trInfoConnection = new DAO_Sqlite(NULL,trRecord_linkName);

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
    if(!thePtr->patientInfoConnection->isDataBaseOpened()){
        this->patientInfoReConnect();
    }

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
    DAO_Mysql* mysqlConnection = new DAO_Mysql(NULL,patientInfo_linkName_MySql);
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
            DAO_Sqlite* sqliteConnection = new DAO_Sqlite(NULL,patientInfo_linkName_Sqlite);
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

        /*link online success sync between local and online*/
        this->sync_PatientInfo();
    }
}

void DAO::sync_PatientInfo(){
    if(!this->patientInfoConnection || this->patientInfoConnection->getTheLinkName() != patientInfo_linkName_MySql){
        QMessageBox::information(nullptr,"Warning","Online database linked failed, the sync from remote to local would be canceled");
        return;
    }

    DAO_Sqlite* sqliteConnection = new DAO_Sqlite(NULL,patientInfo_linkName_Sqlite);
    if(!sqliteConnection->isDataBaseOpened()){
        QMessageBox::information(nullptr, "Error","Open sqlite error");
        exit(-1);
    }

    this->DoSync_PatientInfo_BetweenReomteAndLocal(dynamic_cast<DAO_Mysql*>(this->patientInfoConnection),sqliteConnection);

    sqliteConnection->close();
    delete sqliteConnection;
    sqliteConnection = NULL;
}

void DAO::DoSync_PatientInfo_BetweenReomteAndLocal(DAO_Mysql* remote,DAO_Sqlite* local){
    QSqlQuery query_remote(*remote->getTheDataBase());
    QSqlQuery query_local(*local->getTheDataBase());

    if(!remote->tableExisted(patientInfo_TableName)){
        remote->createEmptyTable_Patient();
    }

    if(!local->tableExisted(patientInfo_TableName)){
        local->createEmptyTable_Patient();
    }




    if(remote->needToUpdateTable_Patient(*ConfigLoader::getInstance()->getThePatientInfoPatten())){
        remote->updateTable_Patient();
    }

    query_remote.exec(QString("lock table %1 write;").arg(patientInfo_TableName));
    if(QSqlError::NoError != query_remote.lastError().type()){
        QMessageBox::critical(nullptr,"Error",query_remote.lastError().text());
        exit(-1);
    }









    query_remote.exec(QString("unlock tables;"));
    if(QSqlError::NoError != query_remote.lastError().type()){
        QMessageBox::critical(nullptr,"Error",query_remote.lastError().text());
        exit(-1);
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
