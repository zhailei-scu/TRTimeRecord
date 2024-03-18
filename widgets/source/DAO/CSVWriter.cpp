#include "../../include/DAO/CSVWriter.h"
#include <QDebug>
#include <QMessageBox>
#include <string>
#include <iostream>
#include <algorithm>

static QString systemCSVPath = "TR.csv";

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

CSVWriter* CSVWriter::thePtr = nullptr;
CSVWriter::GbClear CSVWriter::m_GbClear;

CSVWriter::CSVWriter(){
    this->clear();

    ofs.open(systemCSVPath.toStdString().c_str(),std::ios::out);

    if(!ofs.is_open()){
        QMessageBox::critical(nullptr,"Error",QString("File: %1 opend error!").arg(systemCSVPath));
        exit(-1);
    }

    this->lastLabel = findLastLabel();
}

CSVWriter::~CSVWriter(){
    this->clear();
}

CSVWriter * CSVWriter::getInstance(){
    if(!thePtr){
        thePtr = new CSVWriter();
    }
    return thePtr;
}

void CSVWriter::Start(){
    getInstance();
    qDebug()<<"Csv Started...";
}

void CSVWriter::appendARecord(const QString & tableName,
                              const std::map<unsigned int,QString> & patientInfos,
                              const std::map<unsigned int,QString> & operatorTimes){

    if((QString("#").append(tableName)) != this->lastLabel){
        this->ofs<<QString("#").append(this->lastLabel).toStdString()<<std::endl;
    }

    const std::map<unsigned int,QString> * patientInfoPatten = ConfigLoader::getInstance()->getThePatientInfoPatten();
    for(std::map<unsigned int,QString>::const_iterator it = patientInfoPatten->begin(); it != patientInfoPatten->end(); it++){
        str.append(", ").append(it->second);
    }

    const std::map<unsigned int,QString> * operatorPatten = ConfigLoader::getInstance()->getTheOperatorPatten();
    for(std::map<unsigned int,QString>::const_iterator it = operatorPatten->begin(); it != operatorPatten->end(); it++){
        str.append(", ").append(it->second).append("Time");
    }

    this->ofs.flush();
}

void CSVWriter::deleteLastRecord(const QString & tableName,
                                 const std::map<unsigned int,QString> & patientInfos,
                                 const std::map<unsigned int,QString> & operatorTimes){

}

QString CSVWriter::findLastLabel(){
    QString result;
    std::ifstream ifs;
    char tempChar;
    std::string tempLine;

    ifs.open(systemCSVPath.toStdString().c_str(),std::ios::in);
    if(ifs.is_open()){
        std::streampos size = ifs.tellg();
        for(int i=1;i<size;i++){
            ifs.seekg(-i,std::ios::end);
            ifs.get(tempChar);

            if(0 == tempChar) continue;

            if('\n' == tempChar){
                if(tempLine.size() > 0){
                    std::reverse(tempLine.begin(),tempLine.end());
                    tempLine.erase(0,tempLine.find_first_not_of(" "));
                    tempLine.erase(tempLine.find_last_not_of(" ")+1);

                    if('#' == tempLine.at(0)){
                        result.fromStdString(tempLine);
                        break;
                    }
                }
                tempLine = "";
            }else{
                tempLine += tempChar;
            }

            tempChar = 0;
        }

    }else{
        QMessageBox::critical(nullptr,"Error",QString("File: %1 opend error!").arg(systemCSVPath));
        exit(-1);
    }

    ifs.close();

    return result;
}

void CSVWriter::clear(){
    if(ofs.is_open()){
        ofs.close();
    }

    this->lastLabel = "";
}

/*Garbge clear*/
CSVWriter::GbClear::GbClear(){

}

CSVWriter::GbClear::~GbClear(){
    if(thePtr){
        delete thePtr;
        thePtr = NULL;
    }
}
