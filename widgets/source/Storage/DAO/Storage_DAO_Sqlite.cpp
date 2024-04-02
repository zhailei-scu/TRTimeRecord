#include "../../../include/Storage/DAO/Storage_DAO_Sqlite.h"
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


DAO_Sqlite::DAO_Sqlite(QWidget* parent,const QString & linkName):DAO_Interface(parent,linkName){
    QImage *img = new QImage();
    img->load(":/img/link.svg");
    QLabel* label = new QLabel();

    this->setWindowTitle("Connecting to local database...");
    this->setWindowIcon(QIcon(":/img/logo.ico"));
    this->setWindowFlags(Qt::WindowTitleHint);
    //this->setStyleSheet("background-color: #6fcade");

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

    label->setPixmap(QPixmap::fromImage(*img).scaled(label->size()));

    this->show();
    this->clear();
    this->theDataBase = new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE",linkName));
    this->theDataBase->setDatabaseName(systemDBPath);
    this->theDataBase->open();
    this->hide();
}

DAO_Sqlite::~DAO_Sqlite(){
    this->clear();
}

void DAO_Sqlite::clear(){
    if(this->theDataBase){
        this->theDataBase->close();
        this->theDataBase->removeDatabase(systemDBPath);
        delete this->theDataBase;
        this->theDataBase = NULL;
    }
}

/*Realize the interface*/
bool DAO_Sqlite::isDataBaseOpened(){
    return NULL != this->theDataBase &&
           this->theDataBase->isValid() &&
           this->theDataBase->isOpen();
}

bool DAO_Sqlite::tableExisted(const QString & tableName){
    bool result = false;
    QSqlQuery query(QString("select tbl_name from sqlite_master WHERE type = 'table' AND tbl_name = '%1';").arg(tableName),*this->theDataBase);
    query.exec();
    qDebug()<<query.lastError();
    if(query.next()){
        if(query.value(0).isValid() && query.value(0).toString() == tableName){
            result = true;
        }
    }

    return result;
}

std::list<QString> DAO_Sqlite::getAllTablesName(){
    std::list<QString> result;

    QSqlQuery query(QString("select tbl_name from sqlite_master WHERE type = 'table';"),*this->theDataBase);
    query.exec();
    qDebug()<<query.lastError();
    while(query.next()){
        result.push_back(query.value(0).toString());
    }

    return result;
}

std::list<QString> DAO_Sqlite::getLikelyTablesName(const QString & tableName){
    std::list<QString> result;

    QSqlQuery query(QString("select tbl_name from sqlite_master WHERE type = 'table' and tbl_name like '%1\%';").arg(tableName),*this->theDataBase);
    query.exec();
    qDebug()<<query.lastError();
    while(query.next()){
        result.push_back(query.value(0).toString());
    }

    return result;
}

QString DAO_Sqlite::getRowCount(const QString & tableName){
    QString result("0");
    QSqlQuery query(QString("select count (*) from %1 ;").arg(tableName),*this->theDataBase);
    query.exec();
    qDebug()<<query.lastError();
    if(query.next()){
        if(query.value(0).isValid()){
            result = query.value(0).toString();
        }
    }
    return result;
}

void DAO_Sqlite::createEmptyTable(const QString & tableName){
    QString str("create table ");
    str.append(tableName).append("(id int primary key");

    const std::map<unsigned int,patientInfoPair> * patientInfoPatten = ConfigLoader::getInstance()->getThePatientInfoPatten();
    for(std::map<unsigned int,patientInfoPair>::const_iterator it = patientInfoPatten->begin();
        it != patientInfoPatten->end();
        it++){
        str.append(", ").append(it->second.second).append(" varchar(50)");
    }

    const std::map<unsigned int,OneOperationPattern> * operatorPatten = ConfigLoader::getInstance()->getTheOperationPatten();
    for(std::map<unsigned int,OneOperationPattern>::const_iterator it = operatorPatten->begin(); it != operatorPatten->end(); it++){
        str.append(", ").append(it->second.buttonName).append("Time varchar(50)");
        str.append(", ").append(it->second.buttonName).append("_Pause").append("Time varchar(50)");
        str.append(", ").append(it->second.buttonName).append("_Continue").append("Time varchar(50)");
    }
    str.append(");");

    QSqlQuery query(str,*this->theDataBase);
    query.exec();
    qDebug()<<query.lastError();
}

void DAO_Sqlite::appendARow(const QString & tableName,
                     const std::map<unsigned int,QString> & patientInfos,
                     const std::map<unsigned int,QString> & operatorTimes){
    QString count;
    if(!this->tableExisted(tableName)){
        qDebug()<<"Table is not existed, create a new table: "<<tableName;
        this->createEmptyTable(tableName);
        count = "0";
    }else{
        count = getRowCount(tableName);
    }

    QString str("INSERT INTO ");
    str.append(tableName).append(" (id");
    const std::map<unsigned int,patientInfoPair> * patientInfoPatten = ConfigLoader::getInstance()->getThePatientInfoPatten();
    for(std::map<unsigned int,patientInfoPair>::const_iterator it = patientInfoPatten->begin(); it != patientInfoPatten->end(); it++){
        str.append(", ").append(it->second.second);
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

    QSqlQuery query(str,*this->theDataBase);
    query.exec();
    if(QSqlError::NoError != query.lastError().type()){
        QMessageBox::critical(nullptr,"Error",query.lastError().text());
        exit(-1);
    }
}

void DAO_Sqlite::deleteLastRecord(const QString & tableName){
    QString str("");
    if(this->tableExisted(tableName)){
        str = str.append("delete from %1 where id like ("
                                                     "select id from %1 order by id desc limit 1"
                                                     ")"
                         ";").arg(tableName);
        QSqlQuery query(str,*this->theDataBase);
        query.exec();

        if(QSqlError::NoError != query.lastError().type()){
            QMessageBox::critical(nullptr,"Error",query.lastError().text());
            exit(-1);
        }
    }
}

void DAO_Sqlite::updateTableName(QString & tableName,
                          const std::map<unsigned int,patientInfoPair> & patientPattern,
                          const std::map<unsigned int,OneOperationPattern> & OperationPattern){
    std::stringstream ss;
    int value = 0;
    std::string str_value;
    std::list<QString> list;
    bool flag = false;
    signed int pos = 0;
    std::list<QString> tableNameList = getLikelyTablesName(tableName);

    if(tableNameList.size() > 0){
        tableName = tableNameList.back();
    }

    list.push_back("id");
    for(std::map<unsigned int,patientInfoPair>::const_iterator it = patientPattern.begin();
                                                               it != patientPattern.end();
                                                               it++){
        list.push_back(it->second.second);
    }

    for(std::map<unsigned int,OneOperationPattern>::const_iterator it = OperationPattern.begin();
                                                                   it != OperationPattern.end();
                                                                   it++){
        list.push_back(it->second.buttonName + "Time");
        list.push_back(it->second.buttonName + "_Pause" + "Time");
        list.push_back(it->second.buttonName + "_Continue" + "Time");
    }

    if(this->tableExisted(tableName)){
        QSqlQuery query(QString("PRAGMA table_info(%1)").arg(tableName),*this->theDataBase);
        query.exec();
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
