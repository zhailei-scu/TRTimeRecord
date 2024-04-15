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

void DAO::MergeBasedOnFirst(QString & first,
                            const std::map<QString,QString> & localColumnNames_Ori,
                            const std::map<QString,QString> & localColumnNames,
                            ManualMark mark_Local,
                            const QString & second,
                            const std::map<QString,QString> & remoteColumnNames_Ori,
                            const std::map<QString,QString> & remoteColumnNames,
                            ManualMark mark_Remote){
    QStringList firstSplited = first.split(",");
    QStringList secondSplited = second.split(",");
    unsigned int index = 0;

    if(firstSplited.size() != localColumnNames.size() || secondSplited.size() != remoteColumnNames.size() ||
        localColumnNames.size() != remoteColumnNames.size()){
        QMessageBox::critical(nullptr,"Error","Size checked failed");
        exit(-1);
    }

    for(std::map<QString,QString>::const_iterator it = localColumnNames.begin();
                                                  it != localColumnNames.end();
                                                  it++){
        if(firstSplited.at(index) != secondSplited.at(index)){
            if(localColumnNames_Ori.count(it->first) > 0){
                if(ManualMark(Passive) == mark_Local){
                    firstSplited[index] = secondSplited.at(index);
                }
            }else{
                firstSplited[index] = secondSplited.at(index);
            }
        }

        index++;
    }
}

void DAO::DoSync_PatientInfo_BetweenReomteAndLocal(DAO_Mysql* remote,DAO_Sqlite* local){
    std::map<QString,QString> remoteColumnNames_Ori;
    std::map<QString,QString> localColumnNames_Ori;
    std::map<QString,QString> remoteColumnNames;
    std::map<QString,QString> localColumnNames;
    QString primaryKey = "";
    std::map<QString,QString> data_Remote;
    std::map<QString,QString> data_Local;
    std::map<QString,QString> data_RemoteManualMark;
    std::map<QString,QString> data_LocalManualMark;
    ManualMark mark_Remote;
    ManualMark mark_Local;
    const std::map<unsigned int,OnePatientPattern> * patientInfoPatten = ConfigLoader::getInstance()->getThePatientInfoPatten();
    QSqlQuery query_remote(*remote->getTheDataBase());
    QSqlQuery query_local(*local->getTheDataBase());

    if(!remote->tableExisted(patientInfo_TableName)){
        remote->createEmptyTable_Patient();
    }else{
        remote->getAllColumnName(patientInfo_TableName,remoteColumnNames_Ori);
    }

    if(!remote->tableExisted(patientInfo_ManualMark_TableName)){
        remote->createEmptyTable_Patient_ManualMark();
    }

    if(!local->tableExisted(patientInfo_TableName)){
        local->createEmptyTable_Patient();
    }else{
        local->getAllColumnName(patientInfo_TableName,localColumnNames_Ori);
    }

    query_remote.exec(QString("lock tables %1 write,%2 write;").arg(patientInfo_TableName).arg(patientInfo_ManualMark_TableName));
    if(QSqlError::NoError != query_remote.lastError().type()){
        QMessageBox::critical(nullptr,"Error",query_remote.lastError().text());
        exit(-1);
    }

    if(remote->needToUpdateTable_Patient(*ConfigLoader::getInstance()->getThePatientInfoPatten())){
        remote->updateTable_Patient(false);
    }

    if(local->needToUpdateTable_Patient(*ConfigLoader::getInstance()->getThePatientInfoPatten())){
        local->updateTable_Patient(false);
    }

    /*syncBack of local column*/
    remote->getAllColumnName(patientInfo_TableName,remoteColumnNames);
    local->getAllColumnName(patientInfo_TableName,localColumnNames);

    for(std::map<QString,QString>::iterator it = remoteColumnNames.begin();
                                            it != remoteColumnNames.end();
                                            it++){
        std::map<QString,QString>::iterator it_find = localColumnNames.find(it->first);
        if(localColumnNames.end() == it_find){
            local->insertACol_Patient(it->first,it->second,false);
        }
    }

    for(std::map<QString,QString>::iterator it = localColumnNames.begin();
                                            it != localColumnNames.end();
                                            it++){
        std::map<QString,QString>::iterator it_find = remoteColumnNames.find(it->first);
        if(remoteColumnNames.end() == it_find){
            remote->insertACol_Patient(it->first,it->second,false);
        }
    }

    /*check column between remote and local again*/
    std::map<QString,QString>().swap(remoteColumnNames);
    remoteColumnNames.clear();
    remote->getAllColumnName(patientInfo_TableName,remoteColumnNames);
    std::map<QString,QString>().swap(localColumnNames);
    localColumnNames.clear();
    local->getAllColumnName(patientInfo_TableName,localColumnNames);

    for(std::map<QString,QString>::iterator it = remoteColumnNames.begin();
                                            it != remoteColumnNames.end();
                                            it++){
        std::map<QString,QString>::iterator it_find = localColumnNames.find(it->first);
        if(localColumnNames.end() == it_find){
            QMessageBox::critical(nullptr,"Error",QString("The lack of defination of column %1 in local database even after sync").arg(it->first));
            exit(-1);
        }
    }

    for(std::map<QString,QString>::iterator it = localColumnNames.begin();
                                            it != localColumnNames.end();
                                            it++){
        std::map<QString,QString>::iterator it_find = remoteColumnNames.find(it->first);
        if(remoteColumnNames.end() == it_find){
            QMessageBox::critical(nullptr,"Error",QString("The lack of defination of column %1 in remote database even after sync").arg(it->first));
            exit(-1);
        }
    }

    for(std::map<unsigned int,OnePatientPattern>::const_iterator it = patientInfoPatten->begin();
                                                                 it != patientInfoPatten->end();
                                                                 it++){
        if(it->second.primaryKey){
            primaryKey = it->second.infoName;
            break;
        }
    }

    if("" == primaryKey){
        QMessageBox::critical(nullptr,"Error","The primary key is not finded!");
        exit(-1);
    }

    /*sync between remote and local*/
    remote->getMultiColData_Patient(primaryKey,localColumnNames,",","'","'",data_Remote);
    local->getMultiColData_Patient(primaryKey,localColumnNames,",","'","'",data_Local);

    remote->getManualMark(primaryKey,data_RemoteManualMark);
    local->getManualMark(primaryKey,data_LocalManualMark);
    for(std::map<QString,QString>::iterator it = data_Remote.begin();it != data_Remote.end();it++){
        std::map<QString,QString>::iterator it_find = data_Local.find(it->first);
        if(data_Local.end() != it_find){
            std::map<QString,QString>::iterator it_ManualMark_Remote = data_RemoteManualMark.find(it->first);
            mark_Remote = it_ManualMark_Remote == data_RemoteManualMark.end() ? ManualMark(Passive): ManualMark(it_ManualMark_Remote->second.toInt());
            std::map<QString,QString>::iterator it_ManualMark_Local = data_LocalManualMark.find(it->first);
            mark_Local = it_ManualMark_Local == data_LocalManualMark.end() ? ManualMark(Passive): ManualMark(it_ManualMark_Local->second.toInt());

            qDebug()<<"Need update between "<<it->first<<"   "<<it->second;
            if(it->second != it_find->second){
                this->MergeBasedOnFirst(it_find->second,
                                        localColumnNames_Ori,
                                        localColumnNames,
                                        mark_Local,
                                        it->second,
                                        remoteColumnNames_Ori,
                                        remoteColumnNames,
                                        mark_Remote);
                local->updateARow_Patient(primaryKey,it_find->first,localColumnNames,it_find->second,ManualMark(Passive),false);
                remote->updateARow_Patient(primaryKey,it_find->first,localColumnNames,it_find->second,ManualMark(Passive),false);
            }

            data_Local.erase(it_find);
        }else{
            qDebug()<<"Need insert to local "<<it->first<<"   "<<it->second;
            local->appendARow_Patient(primaryKey,it->first,localColumnNames,it->second,ManualMark(Passive),false);
        }
    }

    /*sync reminded items from local to remote*/
    for(std::map<QString,QString>::iterator it = data_Local.begin();it != data_Local.end();it++){
        qDebug()<<"Need insert to remote "<<it->first<<"   "<<it->second;
        remote->appendARow_Patient(primaryKey,it->first,localColumnNames,it->second,ManualMark(Passive),false);
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
        DAO::getInstance()->getPatientInfoConnection()->updateTable_Patient(true);
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
