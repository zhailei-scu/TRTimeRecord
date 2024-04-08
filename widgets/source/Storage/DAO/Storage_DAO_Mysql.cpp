#include "../../../include/Storage/DAO/Storage_DAO_Mysql.h"
#include "../../../include/Global/Config/Global_Config_ConfigLoader.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QApplication>
#include <QScreen>
#include <QImage>
#include <QLabel>
#include <sstream>
#include <QSqlRecord>
#include <QSqlField>
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
    QSqlQuery query(*this->theDataBase);
    query.exec(QString("select table_name from information_schema.TABLES WHERE table_name = '%1';").arg(tableName));
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
    QSqlQuery query(*this->theDataBase);
    query.exec(QString("select table_name from information_schema.TABLES;"));
    qDebug()<<query.lastError();
    while(query.next()){
        result.push_back(query.value(0).toString());
    }

    return result;
}

std::list<QString> DAO_Mysql::getLikelyTablesName(const QString & tableName){
    std::list<QString> result;
    QSqlQuery query(*this->theDataBase);
    query.exec(QString("select table_name from information_schema.TABLES WHERE table_name like '%1\%';").arg(tableName));
    qDebug()<<query.lastError();
    while(query.next()){
        result.push_back(query.value(0).toString());
    }

    return result;
}

QString DAO_Mysql::getRowCount(const QString & tableName){
    QString result("0");
    QSqlQuery query(*this->theDataBase);
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
    unsigned int index = 0;
    str.append(patientInfo_TableName).append(" (");

    const std::map<unsigned int,OnePatientPattern> * patientInfoPatten = ConfigLoader::getInstance()->getThePatientInfoPatten();
    for(std::map<unsigned int,OnePatientPattern>::const_iterator it = patientInfoPatten->begin();
         it != patientInfoPatten->end();
         it++){
        str.append(it->second.infoName).append(" varchar(50) ");
        if(it->second.primaryKey){
            str.append(" primary key unique");
        }
        if(patientInfoPatten->size() != (index+1)){
            str.append(", ");
        }

        index++;
    }

    str.append(");");

    query.exec(str);
    qDebug()<<query.lastError();
}

void DAO_Mysql::appendARow_TR(const QString & tableName,
                               const std::map<unsigned int,std::pair<QString,QString>> & patientInfos,
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
            str.append(", '").append(it_find->second.second).append("'");
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

void DAO_Mysql::appendARow_Patient(const std::map<unsigned int,std::pair<QString,QString>> & patientInfos){
    std::map<QString,QString>  list;
    bool flag = true;
    QSqlQuery query(*this->theDataBase);
    const std::map<unsigned int,OnePatientPattern> * patientInfoPatten = ConfigLoader::getInstance()->getThePatientInfoPatten();
    unsigned int index = 0;
    if(!this->tableExisted(patientInfo_TableName)){
        qDebug()<<"Table is not existed, create a new table: "<<patientInfo_TableName;
        this->createEmptyTable_Patient();
    }

    /*
    QString str("INSERT INTO ");
    str.append(patientInfo_TableName).append(" (id");
    const std::map<unsigned int,OnePatientPattern> * patientInfoPatten = ConfigLoader::getInstance()->getThePatientInfoPatten();
    for(std::map<unsigned int,OnePatientPattern>::const_iterator it = patientInfoPatten->begin(); it != patientInfoPatten->end(); it++){
        str.append(", ").append(it->second.infoName);
    }

    str.append(") VALUES (").append(count);
    */

    for(std::map<unsigned int,OnePatientPattern>::const_iterator it = patientInfoPatten->begin();
         it != patientInfoPatten->end();
         it++){
        if(it->second.primaryKey){
            auto it_find = patientInfos.find(it->first);
            if(it_find != patientInfos.end()){

                this->getRowValueByItemValue_Patient(it->second.infoName,it_find->second.second,list);

                if(list.size() == patientInfos.size()){
                    for(auto it_rep = patientInfos.begin();it_rep != patientInfos.end();it_rep++){

                        if(list.count(it_rep->second.first) <=0 || list.at(it_rep->second.first) != it_rep->second.second){
                            flag = false;
                            break;
                        }
                    }
                }

                if(flag) return;

                query.exec(QString("DELETE FROM %1 WHERE %2 = '%3';")
                               .arg(patientInfo_TableName)
                               .arg(it->second.infoName)
                               .arg(it_find->second.second));

                qDebug()<<query.lastError();
                break;
            }
        }
    }

    QString str("INSERT INTO ");
    str.append(patientInfo_TableName).append(" (");

    for(std::map<unsigned int,OnePatientPattern>::const_iterator it = patientInfoPatten->begin(); it != patientInfoPatten->end(); it++){
        str.append(it->second.infoName);

        if(patientInfoPatten->size() != (index+1)){
            str.append(",");
        }
        index++;
    }
    str.append(") VALUES (");

    index = 0;
    for(auto it = patientInfoPatten->begin(); it != patientInfoPatten->end();it++){
        auto it_find = patientInfos.find(it->first);
        if(it_find == patientInfos.end()){
            str.append("'").append("").append("'");
        }else{
            str.append("'").append(it_find->second.second).append("'");
        }

        if(patientInfoPatten->size() != (index+1)){
            str.append(",");
        }

        index++;
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

bool DAO_Mysql::columnExisted_TR(const QString & colName) const{
    bool result = false;
    QSqlQuery query(*this->theDataBase);

    query.exec(QString("SELECT COLUMN_NAME FROM information_schema.COLUMNS where table_name='%1';").arg(patientInfo_TableName));
    qDebug()<<query.lastError();

    while(query.next()){
        if(query.value(0).isValid()){
            if(query.value(0).toString() == colName){
                result = true;
                break;
            }
        }
    }

    return result;
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
    std::list<QString> list;
    bool flag = false;

    QSqlQuery query(*this->theDataBase);

    for(std::map<unsigned int,OnePatientPattern>::const_iterator it = patientPattern.begin();
         it != patientPattern.end();
         it++){
        list.push_back(it->second.infoName);
    }

    if(tableExisted(patientInfo_TableName)){

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

    }

    return flag;
}

void DAO_Mysql::updateTable_Patient(){
    std::map<QString,unsigned int> oldColumnNames;
    std::map<unsigned int,std::pair<QString,QString>> newColumnNames;
    QSqlQuery query(*this->theDataBase);
    unsigned int index = 0;
    int findedID = -1;
    QString RefColName = "";
    QString oldTableName = patientInfo_TableName;
    oldTableName.append("_back");

    if(tableExisted(oldTableName)){
        query.exec(QString("drop table %1").arg(oldTableName));
    }

    query.exec(QString("alter table %1 rename to %2").arg(patientInfo_TableName)
                   .arg(oldTableName));

    /*create table pp as select id from patientInfo*/
    /*alter table pp add IDCP varchar(30) not null default ""*/

    query.exec(QString("PRAGMA table_info(%1)").arg(oldTableName));
    qDebug()<<query.lastError();
    while(query.next()){
        if(query.value(0).isValid()){
            oldColumnNames.insert(std::pair<QString,unsigned int>(query.value(1).toString(),0));
        }
    }

    this->createEmptyTable_Patient();

    query.exec(QString("PRAGMA table_info(%1)").arg(patientInfo_TableName));
    qDebug()<<query.lastError();
    while(query.next()){
        if(query.value(0).isValid()){
            newColumnNames.insert(std::pair<unsigned int,std::pair<QString,QString>>(index,std::pair<QString,QString>(query.value(1).toString(),query.value(2).toString())));
        }
        index++;
    }

    if(tableExisted(patientInfo_TableName)){
        query.exec(QString("drop table %1").arg(patientInfo_TableName));
    }

    for(std::map<unsigned int,std::pair<QString,QString>>::iterator it = newColumnNames.begin();it != newColumnNames.end();it++){
        if(oldColumnNames.find(it->second.first) != oldColumnNames.end()){
            findedID = it->first;
            RefColName = it->second.first;
            query.exec(QString("create table %1 as select %3 from %2;").arg(patientInfo_TableName).arg(oldTableName).arg(it->second.first));
            break;
        }
    }

    if(QSqlError::NoError != query.lastError().type()){
        QMessageBox::critical(nullptr,"Error",query.lastError().text());
        exit(-1);
    }

    if(findedID >= 0){
        for(std::map<unsigned int,std::pair<QString,QString>>::iterator it = newColumnNames.begin();it != newColumnNames.end();it++){
            if((signed int)it->first != findedID){
                query.exec(QString("alter table %1 add %2 %3;").arg(patientInfo_TableName).arg(it->second.first).arg(it->second.second));
            }
            if(oldColumnNames.find(it->second.first) != oldColumnNames.end()){
                query.exec(QString("update %1 set %3 = (select %3 from %2 where %1.%4=%2.%4);")
                               .arg(patientInfo_TableName)
                               .arg(oldTableName)
                               .arg(it->second.first)
                               .arg(RefColName));
            }
        }
    }else{
        this->createEmptyTable_Patient();
    }

    qDebug()<<query.lastError();
}


void DAO_Mysql::getAllValueByKey_Patient(const QString & key,QStringList & result) const{
    QSqlQuery query(*this->theDataBase);
    query.exec(QString("select %1 from %2;").arg(key).arg(patientInfo_TableName));

    qDebug()<<query.lastError();
    while(query.next()){
        if(query.value(0).isValid()){
            result.push_back(query.value(0).toString());
        }
    }
}

void DAO_Mysql::getRowValueByItemValue_Patient(const QString & key,const QString & value,std::map<QString,QString> & result) const{
    QSqlQuery query(*this->theDataBase);
    unsigned int size = 0;
    query.exec(QString("select * from %1 where %2 = '%3';").arg(patientInfo_TableName).arg(key).arg(value));

    qDebug()<<query.lastError();
    size = query.record().count();
    while(query.next()){
        for(unsigned int i = 0;i<size;i++){
            result.insert(std::pair<QString,QString>(query.record().fieldName(i),query.record().value(i).toString()));
        }
        break;
    }
}
