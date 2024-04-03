#include "../../../include/Storage/DAO/Storage_DAO_Mysql.h"
#include "../../../include/Global/Config/Global_Config_ConfigLoader.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <sstream>
#include <QSqlError>
#include <QMessageBox>
#include <QApplication>
#include <QScreen>
#include <QImage>
#include <QLabel>
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

static QString patientInfoDataBaseName = "tr_patientinfo";

DAO_Mysql::DAO_Mysql(QWidget* parent,const QString & linkName):DAO_Interface(parent,linkName){
    QImage *img = new QImage();
    img->load(":/img/link.svg");
    QLabel* label = new QLabel(this);

    this->setWindowTitle("Connecting to online database...");
    this->setWindowIcon(QIcon(":/img/logo.ico"));
    this->setWindowFlags(Qt::WindowTitleHint);
    this->setStyleSheet("background-color: #aaaaaa");

    if(parent){
        this->setGeometry(parent->geometry().center().x() - parent->geometry().width()*0.1,
                          parent->geometry().center().y() - 1,
                          parent->geometry().width()*0.2,
                          1);
    }else{
        this->setGeometry(QApplication::primaryScreen()->geometry().center().x() - QApplication::primaryScreen()->geometry().width()*0.1,
                          QApplication::primaryScreen()->geometry().center().y() - 1,
                          QApplication::primaryScreen()->geometry().width()*0.2,
                          1);
    }
    label->setGeometry(0,0,this->width(),this->height());

    label->setPixmap(QPixmap::fromImage(*img));
    label->show();
    this->show();
    this->update();
    this->repaint();

    this->clear();
    this->theDataBase = new QSqlDatabase(QSqlDatabase::addDatabase("QMYSQL",linkName));
    this->theDataBase->setHostName(ConfigLoader::getInstance()->getOnlineDatabaseInfo().at(OnlineInfoPattern::getInstance()->getDefalutPattern().at(0)));
    this->theDataBase->setPort(ConfigLoader::getInstance()->getOnlineDatabaseInfo().at(OnlineInfoPattern::getInstance()->getDefalutPattern().at(1)).toInt());
    this->theDataBase->setDatabaseName(ConfigLoader::getInstance()->getOnlineDatabaseInfo().at(OnlineInfoPattern::getInstance()->getDefalutPattern().at(2)));
    this->theDataBase->setUserName(ConfigLoader::getInstance()->getOnlineDatabaseInfo().at(OnlineInfoPattern::getInstance()->getDefalutPattern().at(3)));
    this->theDataBase->setPassword(ConfigLoader::getInstance()->getOnlineDatabaseInfo().at(OnlineInfoPattern::getInstance()->getDefalutPattern().at(4)));
    this->theDataBase->open();
    this->hide();
}

DAO_Mysql::~DAO_Mysql(){
    this->clear();
}

void DAO_Mysql::clear(){
    if(this->theDataBase){
        this->theDataBase->close();
        this->theDataBase->removeDatabase(systemDBPath);
        delete this->theDataBase;
        this->theDataBase = NULL;
    }
}

/*Realize the interface*/
bool DAO_Mysql::isDataBaseOpened(){
    return this->theDataBase &&
           this->theDataBase->isValid() &&
           this->theDataBase->isOpen();
}

bool DAO_Mysql::tableExisted(const QString & tableName){
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

std::list<QString> DAO_Mysql::getAllTablesName(){
    std::list<QString> result;

    QSqlQuery query;
    query.exec(QString("select tbl_name from sqlite_master WHERE type = 'table';"));
    qDebug()<<query.lastError();
    while(query.next()){
        result.push_back(query.value(0).toString());
    }

    return result;
}

std::list<QString> DAO_Mysql::getLikelyTablesName(const QString & tableName){
    std::list<QString> result;

    QSqlQuery query;
    query.exec(QString("select tbl_name from sqlite_master WHERE type = 'table' and tbl_name like '%1\%';").arg(tableName));
    qDebug()<<query.lastError();
    while(query.next()){
        result.push_back(query.value(0).toString());
    }

    return result;
}

QString DAO_Mysql::getRowCount(const QString & tableName){
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

void DAO_Mysql::createEmptyTable_TR(const QString & tableName){
    QString str("create table ");
    QSqlQuery query(*this->theDataBase);
    str.append(tableName).append("(id int primary key");

    const std::map<unsigned int,OnePatientPattern> * patientInfoPatten = ConfigLoader::getInstance()->getThePatientInfoPatten();
    for(std::map<unsigned int,OnePatientPattern>::const_iterator it = patientInfoPatten->begin();
         it != patientInfoPatten->end();
         it++){
        str.append(", ").append(it->second.infoName).append(" varchar(50)");
    }

    const std::map<unsigned int,OneOperationPattern> * operatorPatten = ConfigLoader::getInstance()->getTheOperationPatten();
    for(std::map<unsigned int,OneOperationPattern>::const_iterator it = operatorPatten->begin(); it != operatorPatten->end(); it++){
        str.append(", ").append(it->second.buttonName).append("Time varchar(50)");
        str.append(", ").append(it->second.buttonName).append("_Pause").append("Time varchar(50)");
        str.append(", ").append(it->second.buttonName).append("_Continue").append("Time varchar(50)");
    }
    str.append(");");

    query.exec(str);
    qDebug()<<query.lastError();
}

void DAO_Mysql::createEmptyTable_Patient(){
    QString str("create table ");
    QSqlQuery query(*this->theDataBase);
    str.append(patientInfo_TableName).append("(id int primary key");

    const std::map<unsigned int,OnePatientPattern> * patientInfoPatten = ConfigLoader::getInstance()->getThePatientInfoPatten();
    for(std::map<unsigned int,OnePatientPattern>::const_iterator it = patientInfoPatten->begin();
         it != patientInfoPatten->end();
         it++){
        str.append(", ").append(it->second.infoName).append(" varchar(50)");
    }

    str.append(");");

    query.exec(str);
    qDebug()<<query.lastError();
}

void DAO_Mysql::appendARow_TR(const QString & tableName,
                              const std::map<unsigned int,QString> & patientInfos,
                              const std::map<unsigned int,QString> & operatorTimes){
    QSqlQuery query(*this->theDataBase);
    QString count;
    if(!this->tableExisted(tableName)){
        qDebug()<<"Table is not existed, create a new table: "<<tableName;
        this->createEmptyTable_TR(tableName);
        count = "0";
    }else{
        count = getRowCount(tableName);
    }

    QString str("INSERT INTO ");
    str.append(tableName).append(" (id");
    const std::map<unsigned int,OnePatientPattern> * patientInfoPatten = ConfigLoader::getInstance()->getThePatientInfoPatten();
    for(std::map<unsigned int,OnePatientPattern>::const_iterator it = patientInfoPatten->begin(); it != patientInfoPatten->end(); it++){
        str.append(", ").append(it->second.infoName);
    }

    const std::map<unsigned int,OneOperationPattern> * operatorPatten = ConfigLoader::getInstance()->getTheOperationPatten();
    for(std::map<unsigned int,OneOperationPattern>::const_iterator it = operatorPatten->begin(); it != operatorPatten->end(); it++){
        str.append(", ").append(it->second.buttonName).append("Time");
        str.append(", ").append(it->second.buttonName).append("_Pause").append("Time");
        str.append(", ").append(it->second.buttonName).append("_Continue").append("Time");
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

    for(auto it = operatorPatten->begin();it != operatorPatten->end();it++){
        for(unsigned int i = it->first*3;i<(it->first+1)*3;i++){
            auto it_find = operatorTimes.find(i);
            if(it_find == operatorTimes.end()){
                str.append(", '").append("").append("'");
            }else{
                str.append(", '").append(it_find->second).append("'");
            }
        }
    }

    str.append(");");

    query.exec(str);
    if(QSqlError::NoError != query.lastError().type()){
        QMessageBox::critical(nullptr,"Error",query.lastError().text());
        exit(-1);
    }
}

void DAO_Mysql::appendARow_Patient(const std::map<unsigned int,QString> & patientInfos){
    QSqlQuery query(*this->theDataBase);
    QString count;
    if(!this->tableExisted(patientInfo_TableName)){
        qDebug()<<"Table is not existed, create a new table: "<<patientInfo_TableName;
        this->createEmptyTable_Patient();
        count = "0";
    }else{
        count = getRowCount(patientInfo_TableName);
    }

    QString str("INSERT INTO ");
    str.append(patientInfo_TableName).append(" (id");
    const std::map<unsigned int,OnePatientPattern> * patientInfoPatten = ConfigLoader::getInstance()->getThePatientInfoPatten();
    for(std::map<unsigned int,OnePatientPattern>::const_iterator it = patientInfoPatten->begin(); it != patientInfoPatten->end(); it++){
        str.append(", ").append(it->second.infoName);
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

    str.append(");");

    query.exec(str);
    if(QSqlError::NoError != query.lastError().type()){
        QMessageBox::critical(nullptr,"Error",query.lastError().text());
        exit(-1);
    }
}

void DAO_Mysql::deleteLastRecord(const QString & tableName){
    QString str("");
    QSqlQuery query(*this->theDataBase);
    if(this->tableExisted(tableName)){
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

void DAO_Mysql::updateTableName_TR(QString & tableName,
                                    const std::map<unsigned int,OnePatientPattern> & patientPattern,
                                    const std::map<unsigned int,OneOperationPattern> & OperationPattern){
    std::stringstream ss;
    int value = 0;
    std::string str_value;
    std::list<QString> list;
    bool flag = false;
    signed int pos = 0;
    std::list<QString> tableNameList = getLikelyTablesName(tableName);
    QSqlQuery query(*this->theDataBase);

    if(tableNameList.size() > 0){
        tableName = tableNameList.back();
    }

    list.push_back("id");
    for(std::map<unsigned int,OnePatientPattern>::const_iterator it = patientPattern.begin();
         it != patientPattern.end();
         it++){
        list.push_back(it->second.infoName);
    }

    for(std::map<unsigned int,OneOperationPattern>::const_iterator it = OperationPattern.begin();
         it != OperationPattern.end();
         it++){
        list.push_back(it->second.buttonName + "Time");
        list.push_back(it->second.buttonName + "_Pause" + "Time");
        list.push_back(it->second.buttonName + "_Continue" + "Time");
    }

    if(this->tableExisted(tableName)){
        query.exec(QString("PRAGMA table_info(%1)").arg(tableName));
        qDebug()<<query.lastError();
        while(query.next()){
            if(query.value(0).isValid()){
                if(0 == list.size()){
                    flag = true;
                    break;
                }

                if(query.value(1).toString() != list.front()){
                    flag = true;
                    break;
                }
                list.pop_front();
            }
        }

        if(list.size() > 0){
            flag = true;
        }
    }

    if(flag){
        pos = tableName.toStdString().find_first_of(appendFlag);

        if(pos>=0){
            ss.str("");
            ss.clear();
            ss<<tableName.toStdString().substr(pos+1,tableName.size()-1);
            ss>>value;
            ss.str("");
            ss.clear();
            ss<<value + 1;
            ss>>str_value;
            tableName = tableName.toStdString().substr(0,pos+1).c_str();
            tableName.append(str_value);
        }else{
            tableName.append("_").append(appendFlag).append("1");
        }
    }
}

bool DAO_Mysql::needToUpdateTable_Patient(const std::map<unsigned int,OnePatientPattern> & patientPattern){
    std::stringstream ss;
    std::string str_value;
    std::list<QString> list;
    bool flag = false;
    signed int pos = 0;

    QSqlQuery query(*this->theDataBase);

    list.push_back("id");
    for(std::map<unsigned int,OnePatientPattern>::const_iterator it = patientPattern.begin();
                                                                 it != patientPattern.end();
                                                                 it++){
        list.push_back(it->second.infoName);
    }

    query.exec(QString("PRAGMA table_info(%1)").arg(patientInfo_TableName));
    qDebug()<<query.lastError();
    while(query.next()){
        if(query.value(0).isValid()){
            if(0 == list.size()){
                flag = true;
                break;
            }

            if(query.value(1).toString() != list.front()){
                flag = true;
                break;
            }
            list.pop_front();
        }
    }

    if(list.size() > 0){
        flag = true;
    }

    return flag;
}

void DAO_Mysql::updateTable_Patient(){
    std::map<QString,unsigned int> oldColumnIndexMap;
    std::map<unsigned int,int> indexMap;
    QSqlQuery query(*this->theDataBase);
    QString oldTableName = patientInfo_TableName;

    unsigned int index = 0;
    oldTableName.append("_back");

    query.exec(QString("alter table %1 rename to %2").arg(patientInfo_TableName)
                   .arg(oldTableName));
    this->createEmptyTable_Patient();

    query.exec(QString("PRAGMA table_info(%1)").arg(oldTableName));
    qDebug()<<query.lastError();
    while(query.next()){
        if(query.value(0).isValid()){
            oldColumnIndexMap.insert(std::pair<QString,unsigned int>(query.value(1).toString(),index));
        }
        index++;
    }

    const std::map<unsigned int,OnePatientPattern> * patientInfo = ConfigLoader::getInstance()->getThePatientInfoPatten();

    if(patientInfo){
        for(std::map<unsigned int,OnePatientPattern>::const_iterator it = patientInfo->begin();
             it != patientInfo->end();
             it++){
            std::map<QString,unsigned int>::iterator it_find = oldColumnIndexMap.find(it->second.infoName);
            if(oldColumnIndexMap.end() != it_find){
                indexMap.insert(std::pair<unsigned int,int>(it->first,it_find->second));
            }else{
                indexMap.insert(std::pair<unsigned int,int>(it->first,-1));
            }
        }
    }

    for(std::map<unsigned int,OnePatientPattern>::const_iterator it = patientInfo->begin();
         it != patientInfo->end();
         it++){
        if(indexMap.at(it->first) > 0){
            query.exec(QString("update %1 set %1.%3 = %2.%3 ;").arg(patientInfo_TableName)
                           .arg(oldTableName)
                           .arg(it->second.infoName));
        }
    }

    qDebug()<<query.lastError();
}
