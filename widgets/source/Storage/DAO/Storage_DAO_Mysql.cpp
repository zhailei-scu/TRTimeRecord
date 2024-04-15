#include "../../../include/Storage/DAO/Storage_DAO_Mysql.h"
#include "../../../include/Common/Util/Common_Util_Base.h"
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

    if(!this->theDataBase || !this->theDataBase->isOpen() || this->theDataBase->isOpenError() || !this->theDataBase->isValid()){
        QMessageBox::information(nullptr,"Information",this->theDataBase->lastError().text());
    }

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
        if(query.value(0).isValid() && query.value(0).toString().toLower() == tableName.toLower()){
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
    QMessageBox::critical(nullptr,"Error","Internal error: query to wrong database.");
    exit(-1);
}

void DAO_Mysql::createEmptyTable_Patient(){
    QString str("create table ");
    QSqlQuery query(*this->theDataBase);
    unsigned int size = 0;
    unsigned int index = 0;
    str.append(patientInfo_TableName).append(" (");

    const std::map<unsigned int,OnePatientPattern> * patientInfoPatten = ConfigLoader::getInstance()->getThePatientInfoPatten();
    size = patientInfoPatten->size();
    for(std::map<unsigned int,OnePatientPattern>::const_iterator it = patientInfoPatten->begin();
                                                                 it != patientInfoPatten->end();
                                                                 it++){
        index++;
        str.append(it->second.infoName).append(" varchar(50) ");
        if(it->second.primaryKey){
            str.append(" primary key unique");
        }
        if(index < size){
            str.append(", ");
        }
    }

    str.append(");");

    query.exec(str);
    qDebug()<<query.lastError();
}

void DAO_Mysql::createEmptyTable_Patient_ManualMark(){
    QString str("create table ");
    bool finded = false;
    QSqlQuery query(*this->theDataBase);
    str.append(patientInfo_ManualMark_TableName).append(" (");

    const std::map<unsigned int,OnePatientPattern> * patientInfoPatten = ConfigLoader::getInstance()->getThePatientInfoPatten();
    for(std::map<unsigned int,OnePatientPattern>::const_iterator it = patientInfoPatten->begin();
                                                                 it != patientInfoPatten->end();
                                                                 it++){
        if(it->second.primaryKey){
            str.append(it->second.infoName).append(" varchar(50) ");
            str.append(" primary key unique,");
            finded = true;
            break;
        }
    }

    if(!finded){
        QMessageBox::critical(nullptr,"Error","No primary key finded!");
        exit(-1);
    }

    str.append("Mark integer");

    str.append(");");

    query.exec(str);
    qDebug()<<query.lastError();
}

void DAO_Mysql::appendARow_TR(const QString & tableName,
                              const std::map<unsigned int,std::pair<QString,QString>> & patientInfos,
                              const std::map<unsigned int,QString> & operatorTimes){
    QMessageBox::critical(nullptr,"Error","Internal error: query to wrong database.");
    exit(-1);
}

void DAO_Mysql::updateARow_Patient(const std::map<unsigned int,std::pair<QString,QString>> & patientInfos,
                                   ManualMark mark,
                                   const bool lock){
    std::map<QString,QString>  list;
    bool flag = true;
    bool finded = false;
    QSqlQuery query(*this->theDataBase);
    const std::map<unsigned int,OnePatientPattern> * patientInfoPatten = ConfigLoader::getInstance()->getThePatientInfoPatten();
    QString str;
    QString primaryKey;
    QString primaryKeyValue;
    if(!this->tableExisted(patientInfo_TableName)){
        qDebug()<<"Table is not existed, create a new table: "<<patientInfo_TableName;
        this->createEmptyTable_Patient();
    }

    if(!this->tableExisted(patientInfo_ManualMark_TableName)){
        qDebug()<<"Table is not existed, create a new table: "<<patientInfo_ManualMark_TableName;
        this->createEmptyTable_Patient_ManualMark();
    }

    if(lock){
        query.exec(QString("lock tables %1 write,%2 write;").arg(patientInfo_TableName).arg(patientInfo_ManualMark_TableName));
        if(QSqlError::NoError != query.lastError().type()){
            QMessageBox::critical(nullptr,"Error",query.lastError().text());
            exit(-1);
        }
    }

    for(std::map<unsigned int,OnePatientPattern>::const_iterator it = patientInfoPatten->begin();
                                                                 it != patientInfoPatten->end();
                                                                 it++){
        if(it->second.primaryKey){
            finded = true;
            auto it_find = std::find_if(patientInfos.begin(),patientInfos.end(),map_value_finder_PairInValue<unsigned int,QString,QString>(it->second.infoName));
            if(it_find != patientInfos.end()){
                primaryKey = it_find->second.first;
                primaryKeyValue = it_find->second.second;

                this->getRowValueByItemValue_Patient(primaryKey,primaryKeyValue,"","","",list);

                if(list.size() == patientInfos.size()){
                    for(auto it_rep = patientInfos.begin();it_rep != patientInfos.end();it_rep++){

                        if(list.count(it_rep->second.first) <= 0 || list.at(it_rep->second.first) != it_rep->second.second){
                            flag = false;
                            break;
                        }
                    }
                }else{
                    flag = false;
                }

                if(flag){

                    if(lock){
                        query.exec(QString("unlock tables;"));
                        if(QSqlError::NoError != query.lastError().type()){
                            QMessageBox::critical(nullptr,"Error",query.lastError().text());
                            exit(-1);
                        }
                    }

                    return;
                }

                query.exec(QString("DELETE FROM %1 WHERE %2 = '%3';")
                               .arg(patientInfo_TableName)
                               .arg(primaryKey)
                               .arg(primaryKeyValue));

                qDebug()<<query.lastError();
                break;
            }else{
                QMessageBox::critical(nullptr,"Error","No primary key finded in inputed patient infos.");
                exit(-1);
            }
        }
    }

    if(!finded){
        QMessageBox::critical(nullptr,"Error","No primary key finded.");
        exit(-1);
    }

    this->updateARow_PatientManualMark(primaryKey,primaryKeyValue,mark);

    this->generateSQL_appendARow_Patient(patientInfos,str);
    query.exec(str);
    if(QSqlError::NoError != query.lastError().type()){
        QMessageBox::critical(nullptr,"Error",query.lastError().text());
        exit(-1);
    }

    if(lock){
        query.exec(QString("unlock tables;"));
        if(QSqlError::NoError != query.lastError().type()){
            QMessageBox::critical(nullptr,"Error",query.lastError().text());
            exit(-1);
        }
    }
}

void DAO_Mysql::updateARow_Patient(const QString & primaryKey,
                                   const QString & primaryValue,
                                   const std::map<QString,QString> & colName,
                                   const QString & values,
                                   ManualMark mark,
                                   bool lock){
    std::map<QString,QString>  list;
    QSqlQuery query(*this->theDataBase);
    QString str;
    if(!this->tableExisted(patientInfo_TableName)){
        qDebug()<<"Table is not existed, create a new table: "<<patientInfo_TableName;
        this->createEmptyTable_Patient();
    }

    if(!this->tableExisted(patientInfo_ManualMark_TableName)){
        qDebug()<<"Table is not existed, create a new table: "<<patientInfo_ManualMark_TableName;
        this->createEmptyTable_Patient_ManualMark();
    }

    if(lock){
        query.exec(QString("lock tables %1 write,%2 write;").arg(patientInfo_TableName).arg(patientInfo_ManualMark_TableName));
        if(QSqlError::NoError != query.lastError().type()){
            QMessageBox::critical(nullptr,"Error",query.lastError().text());
            exit(-1);
        }
    }

    query.exec(QString("DELETE FROM %1 WHERE %2 = '%3';")
                               .arg(patientInfo_TableName)
                               .arg(primaryKey)
                               .arg(primaryValue));

    qDebug()<<query.lastError();

    this->updateARow_PatientManualMark(primaryKey,primaryValue,mark);

    this->generateSQL_appendARow_Patient(colName,values,str);
    query.exec(str);
    if(QSqlError::NoError != query.lastError().type()){
        QMessageBox::critical(nullptr,"Error",query.lastError().text());
        exit(-1);
    }

    if(lock){
        query.exec(QString("unlock tables;"));
        if(QSqlError::NoError != query.lastError().type()){
            QMessageBox::critical(nullptr,"Error",query.lastError().text());
            exit(-1);
        }
    }
}

void DAO_Mysql::appendARow_Patient(const QString & primaryKey,
                                   const QString & primaryValue,
                                   const std::map<QString,QString> & colName,
                                   const QString & values,
                                   ManualMark mark,
                                   bool lock){
    std::map<QString,QString>  list;
    QSqlQuery query(*this->theDataBase);
    QString str;
    if(!this->tableExisted(patientInfo_TableName)){
        qDebug()<<"Table is not existed, create a new table: "<<patientInfo_TableName;
        this->createEmptyTable_Patient();
    }

    if(!this->tableExisted(patientInfo_ManualMark_TableName)){
        qDebug()<<"Table is not existed, create a new table: "<<patientInfo_ManualMark_TableName;
        this->createEmptyTable_Patient_ManualMark();
    }

    if(lock){
        query.exec(QString("lock tables %1 write,%2 write;").arg(patientInfo_TableName).arg(patientInfo_ManualMark_TableName));
        if(QSqlError::NoError != query.lastError().type()){
            QMessageBox::critical(nullptr,"Error",query.lastError().text());
            exit(-1);
        }
    }

    this->updateARow_PatientManualMark(primaryKey,primaryValue,mark);

    this->generateSQL_appendARow_Patient(colName,values,str);
    query.exec(str);
    if(QSqlError::NoError != query.lastError().type()){
        QMessageBox::critical(nullptr,"Error",query.lastError().text().append(str));
        exit(-1);
    }

    if(lock){
        query.exec(QString("unlock tables;"));
        if(QSqlError::NoError != query.lastError().type()){
            QMessageBox::critical(nullptr,"Error",query.lastError().text());
            exit(-1);
        }
    }
}

void DAO_Mysql::updateARow_PatientManualMark(const QString & primaryKey,
                                             const QString & primaryKeyValue,
                                             ManualMark mark){
    QSqlQuery query(*this->theDataBase);
    QString str;

    query.exec(QString("select * from %1 where %2 = '%3';")
                        .arg(patientInfo_ManualMark_TableName)
                        .arg(primaryKey)
                        .arg(primaryKeyValue));

    if(query.size() > 0){
        query.exec(QString("delete from %1 where %2 = '%3';")
                        .arg(patientInfo_ManualMark_TableName)
                        .arg(primaryKey)
                        .arg(primaryKeyValue));
    }

    str.append("INSERT INTO ");
    str.append(patientInfo_ManualMark_TableName).append(" (");
    str.append(primaryKey).append(",");
    str.append("Mark");
    str.append(") VALUES (");
    str.append("'").append(primaryKeyValue).append("'").append(",");
    str.append("'").append(std::to_string((int)mark).c_str()).append("'");
    str.append(");");

    query.exec(str);
    if(QSqlError::NoError != query.lastError().type()){
        QMessageBox::critical(nullptr,"Error",query.lastError().text().append(str));
        exit(-1);
    }
}

void DAO_Mysql::generateSQL_appendARow_Patient(const std::map<unsigned int,std::pair<QString,QString>> & patientInfos,QString & result){
    QString colnumNames;
    QString values;
    unsigned int size = patientInfos.size();
    unsigned int index = 0;
    for(std::map<unsigned int,std::pair<QString,QString>>::const_iterator it = patientInfos.begin(); it != patientInfos.end(); it++){
        index++;
        colnumNames.append(it->second.first);
        values.append("'").append(it->second.second).append("'");
        if(index < size){
            colnumNames.append(",");
            values.append(",");
        }
    }

    this->generateSQL_appendARow_Patient(colnumNames,values,result);
}

void DAO_Mysql::generateSQL_appendARow_Patient(const std::map<QString,QString> & colName,const QString & values,QString & result){
    QString colnumNames;
    unsigned int size = colName.size();
    unsigned int index = 0;
    for(std::map<QString,QString>::const_iterator it = colName.begin(); it != colName.end(); it++){
         index++;
        colnumNames.append(it->first);

         if(index < size){
            colnumNames.append(",");
        }
    }

    this->generateSQL_appendARow_Patient(colnumNames,values,result);
}

void DAO_Mysql::generateSQL_appendARow_Patient(const QString & colNamesCombine,const QString & values,QString & result){
    result.append("INSERT INTO ");
    result.append(patientInfo_TableName).append(" (").append(colNamesCombine);
    result.append(") VALUES (").append(values).append(");");
}

void DAO_Mysql::deleteLastRecord_TR(const QString & tableName){
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

void DAO_Mysql::getAllColumnName(const QString & tableName,
                                 std::map<QString,QString> & result) const{
    QSqlQuery query(*this->theDataBase);

    query.exec(QString("SELECT COLUMN_NAME,COLUMN_TYPE FROM information_schema.COLUMNS where table_name='%1';").arg(tableName));
    qDebug()<<query.lastError();

    while(query.next()){
        if(query.value(0).isValid()){
            result.insert(std::pair<QString,QString>(query.value(0).toString(),query.value(1).toString()));
        }
    }
}

bool DAO_Mysql::columnExisted(const QString & tableName,const QString & colName) const{
    bool result = false;
    QSqlQuery query(*this->theDataBase);

    query.exec(QString("SELECT COLUMN_NAME FROM information_schema.COLUMNS where table_name='%1';").arg(tableName));
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
    QMessageBox::critical(nullptr,"Error","Internal error: query to wrong database.");
    exit(-1);
}

bool DAO_Mysql::needToUpdateTable_Patient(const std::map<unsigned int,OnePatientPattern> & patientPattern){
    std::map<QString,QString> list_PatientPattern;
    std::map<QString,QString> list_TableColumnName;
    bool flag = false;

    for(std::map<unsigned int,OnePatientPattern>::const_iterator it = patientPattern.begin();
                                                                 it != patientPattern.end();
                                                                 it++){
        list_PatientPattern.insert(std::pair<QString,QString>(it->second.infoName,""));
    }

    if(tableExisted(patientInfo_TableName)){
        this->getAllColumnName(patientInfo_TableName,list_TableColumnName);
        for(auto it = list_PatientPattern.begin();it != list_PatientPattern.end();it++){
            if(list_TableColumnName.count(it->first) <= 0){
                flag = true;
                break;
            }
        }
    }

    return flag;
}

void DAO_Mysql::insertACol_Patient(const QString & colName,const QString & colType,bool lock){
    QSqlQuery query(*this->theDataBase);
    if(lock){
        query.exec(QString("lock table %1 write;").arg(patientInfo_TableName));
        if(QSqlError::NoError != query.lastError().type()){
            QMessageBox::critical(nullptr,"Error",query.lastError().text());
            exit(-1);
        }
    }

    query.exec(QString("alter table %1 add %2 %3;").arg(patientInfo_TableName).arg(colName).arg(colType));
    if(QSqlError::NoError != query.lastError().type()){
        QMessageBox::critical(nullptr,"Error",query.lastError().text());
        exit(-1);
    }

    if(lock){
        query.exec(QString("unlock tables;"));
        if(QSqlError::NoError != query.lastError().type()){
            QMessageBox::critical(nullptr,"Error",query.lastError().text());
            exit(-1);
        }
    }
}

void DAO_Mysql::updateTable_Patient(bool lock){
    std::map<QString,QString> columnNames;
    const std::map<unsigned int,OnePatientPattern> * patientPattern = ConfigLoader::getInstance()->getThePatientInfoPatten();
    QSqlQuery query(*this->theDataBase);

    if(!this->tableExisted(patientInfo_TableName)){
        qDebug()<<"Table is not existed, create a new table: "<<patientInfo_TableName;
        this->createEmptyTable_Patient();
        return;
    }

    if(lock){
        query.exec(QString("lock table %1 write;").arg(patientInfo_TableName));
        if(QSqlError::NoError != query.lastError().type()){
            QMessageBox::critical(nullptr,"Error",query.lastError().text());
            exit(-1);
        }
    }

    /*create table pp as select id from patientInfo*/
    /*alter table pp add IDCP varchar(30) not null default ""*/
    this->getAllColumnName(patientInfo_TableName,columnNames);

    for(std::map<unsigned int,OnePatientPattern>::const_iterator it = patientPattern->begin();
                                                                 it != patientPattern->end();
                                                                 it++){
        if(columnNames.find(it->second.infoName) == columnNames.end()){
            query.exec(QString("alter table %1 add %2 varchar(30);").arg(patientInfo_TableName).arg(it->second.infoName));
            if(QSqlError::NoError != query.lastError().type()){
                QMessageBox::critical(nullptr,"Error",query.lastError().text());
                exit(-1);
            }
        }
    }

    if(lock){
        query.exec(QString("unlock tables;"));
        if(QSqlError::NoError != query.lastError().type()){
            QMessageBox::critical(nullptr,"Error",query.lastError().text());
            exit(-1);
        }
    }
}

void DAO_Mysql::getOneColData_Patient(const QString & key,
                                      const QString & seperate,
                                      const QString & preAppendStr,
                                      const QString & postAppendStr,
                                      QStringList & result) const{
    QSqlQuery query(*this->theDataBase);
    unsigned int size = 0;
    unsigned int index = 0;
    query.exec(QString("select %1 from %2;").arg(key).arg(patientInfo_TableName));

    qDebug()<<query.lastError();

    size = query.size();
    while(query.next()){
        index++;

        if(query.value(0).isValid()){

            if(index < size){
                result.push_back(preAppendStr + query.value(0).toString() + postAppendStr + seperate);
            }else{
                result.push_back(preAppendStr + query.value(0).toString() + postAppendStr);
            }
        }
    }
}

void DAO_Mysql::getRowValueByItemValue_Patient(const QString & key,
                                               const QString & value,
                                               const QString & seperate,
                                               const QString & preAppendStr,
                                               const QString & postAppendStr,
                                               std::map<QString,QString> & result) const{
    QSqlQuery query(*this->theDataBase);
    unsigned int size = 0;
    unsigned int index = 0;

    query.exec(QString("select * from %1 where %2 = '%3';").arg(patientInfo_TableName).arg(key).arg(value));

    qDebug()<<query.lastError();
    size = query.record().count();
    while(query.next()){
        for(unsigned int i = 0;i<size;i++){
            index++;

            if(index < size){
                result.insert(std::pair<QString,QString>(query.record().fieldName(i),preAppendStr + query.record().value(i).toString() + postAppendStr + seperate));
            }else{
                result.insert(std::pair<QString,QString>(query.record().fieldName(i),preAppendStr + query.record().value(i).toString() + postAppendStr));
            }
        }
        break;
    }
}

void DAO_Mysql::getMultiColData_Patient(const QString & primaryKey,
                                        const std::map<QString,QString> & columNames,
                                        const QString & seperate,
                                        const QString & preAppendStr,
                                        const QString & postAppendStr,
                                        std::map<QString,QString> & result) const{
    QSqlQuery query(*this->theDataBase);
    QString queryColumn;
    unsigned int size = columNames.size();
    unsigned int index = 0;

    for(std::map<QString,QString>::const_iterator it = columNames.begin();
                                                  it != columNames.end();
                                                  it++){
        index++;

        queryColumn.append(it->first);

        if(index < size){
            queryColumn.append(",");
        }
    }

    this->getMultiColData_Patient(primaryKey,queryColumn,seperate,preAppendStr,postAppendStr,result);
}

void DAO_Mysql::getMultiColData_Patient(const QString & primaryKey,
                                        const QString & columNames,
                                        const QString & seperate,
                                        const QString & preAppendStr,
                                        const QString & postAppendStr,
                                        std::map<QString,QString> & result) const{
    QSqlQuery query(*this->theDataBase);
    unsigned int size = columNames.size();
    unsigned int index = 0;
    QString str = "";

    query.exec(QString("select %1 from %2;").arg(columNames).arg(patientInfo_TableName));

    qDebug()<<"all: "<<query.lastError()<<query.lastQuery();

    while(query.next()){
        size = query.record().count();
        str = "";
        index = 0;
        for(unsigned int i = 0;i<size;i++){
            index++;

            if(index < size){
                str.append(preAppendStr + query.record().value(i).toString() + postAppendStr + seperate);
            }else{
                str.append(preAppendStr + query.record().value(i).toString() + postAppendStr);
            }
        }
        result.insert(std::pair<QString,QString>(query.record().value(primaryKey).toString(),str));
    }
}
