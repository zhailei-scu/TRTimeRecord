#include "../../../include/Storage/CSV/Storage_CSV_Writer.h"
#include "../../../include/Global/Config/Global_Config_ConfigLoader.h"
#include <QDebug>
#include <QMessageBox>
#include <string>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <QFile>
#include <filesystem>

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
    std::ifstream ifs;
    this->clear();

    ofs.open(ConfigLoader::getInstance()->getSystemCSVPath().toStdString().c_str(),std::ios::out | std::ios::app);

    if(!ofs.is_open()){
        QMessageBox::critical(nullptr,"Error",QString("File: %1 opend error!").arg(ConfigLoader::getInstance()->getSystemCSVPath()));
        exit(-1);
    }

    this->lastLabel = findLastLabel();

    qDebug()<<this->lastLabel;

    ifs.open(ConfigLoader::getInstance()->getSystemCSVPath().toStdString().c_str(),std::ios::in);

    if(!ifs.is_open()){
        QMessageBox::critical(nullptr,"Error",QString("File: %1 opend error!").arg(ConfigLoader::getInstance()->getSystemCSVPath()));
        exit(-1);
    }
    ifs.seekg(0,std::ios::end);
    this->lastItemWritePos = ifs.tellg();
    qDebug()<<this->lastItemWritePos;

    ifs.close();
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
                              const std::map<unsigned int,std::pair<QString,QString>> & patientInfos,
                              const std::map<unsigned int,QString> & operatorTimes){

    const std::map<unsigned int,OnePatientPattern> * patientInfoPatten = ConfigLoader::getInstance()->getThePatientInfoPatten();
    const std::map<unsigned int,OneOperationPattern> * operatorPatten = ConfigLoader::getInstance()->getTheOperationPatten();
    QString tableNameNew("#");
    tableNameNew.append(tableName);

    if(tableNameNew != this->lastLabel){
        this->ofs<<tableNameNew.toStdString()<<std::endl;

        for(std::map<unsigned int,OnePatientPattern>::const_iterator it = patientInfoPatten->begin(); it != patientInfoPatten->end(); it++){
            ofs<<std::setw(30)<<it->second.infoName.toStdString()<<",";
            ofs<<std::setw(30)<<"necessary"<<",";
        }

        for(std::map<unsigned int,OneOperationPattern>::const_iterator it = operatorPatten->begin(); it != operatorPatten->end(); it++){
            ofs<<std::setw(30)<<it->second.buttonName.toStdString().append("Time")<<",";
            ofs<<std::setw(30)<<it->second.buttonName.toStdString().append("_Pause").append("Time")<<",";
            ofs<<std::setw(30)<<it->second.buttonName.toStdString().append("_Continue").append("Time")<<",";
        }

        ofs<<std::endl;

        this->lastLabel = tableNameNew;
    }

    ofs.seekp(0,std::ios::end);
    this->lastItemWritePos = ofs.tellp();
    qDebug()<<this->lastItemWritePos;

    for(auto it = patientInfoPatten->begin(); it != patientInfoPatten->end();it++){
        auto it_find = patientInfos.find(it->first);
        if(patientInfos.end() == it_find){
            ofs<<std::setw(30)<<",";
        }else{
           ofs<<std::setw(30)<<it_find->second.second.toStdString()<<",";
        }
    }

    for(auto it = operatorPatten->begin(); it != operatorPatten->end();it++){
        for(unsigned int i = it->first*3;i<(it->first+1)*3;i++){
        auto it_find = operatorTimes.find(i);
            if(operatorTimes.end() == it_find){
                ofs<<std::setw(30)<<",";
            }else{
                ofs<<std::setw(30)<<it_find->second.toStdString()<<",";
            }
        }
    }

    ofs<<std::endl;

    this->ofs.flush();
}

void CSVWriter::deleteLastRecord(){
    /*
    qDebug()<<this->lastItemWritePos;

    this->ofs.seekp(this->lastItemWritePos-1,std::ios::beg);

    this->ofs.write("\n",1);
    */

    if(0 >= this->lastItemWritePos){
        QMessageBox::critical(nullptr,"Fatal Error!","The delete operation will delete file from postion 0, please stop it and contact Lei.Zhai !!!!");
        exit(-1);
    }
    std::filesystem::resize_file(ConfigLoader::getInstance()->getSystemCSVPath().toStdString(),this->lastItemWritePos);
    this->ofs.flush();
}

QString CSVWriter::findLastLabel(){
    QString result;
    std::ifstream ifs;
    char tempChar;
    std::string tempLine;

    ifs.open(ConfigLoader::getInstance()->getSystemCSVPath().toStdString().c_str(),std::ios::ate);
    if(ifs.is_open()){
        ifs.seekg(0,std::ios::end);
        std::streampos size = ifs.tellg();
        for(int i=1;i<size;i++){

            ifs.seekg(-i,std::ios::end);
            ifs.get(tempChar);

            if('\n' == tempChar || size == (i + 1)){
                if(tempLine.size() > 0){

                    std::reverse(tempLine.begin(),tempLine.end());
                    tempLine.erase(0,tempLine.find_first_not_of(" "));
                    tempLine.erase(0,tempLine.find_first_not_of("\xBF"));
                    tempLine.erase(0,tempLine.find_first_not_of("\xEF"));
                    tempLine.erase(0,tempLine.find_first_not_of("\xBB"));
                    tempLine.erase(tempLine.find_last_not_of(" ")+1);

                    if(tempLine.size() > 0 && '#' == tempLine.at(0)){
                        result = tempLine.c_str();
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
        QMessageBox::critical(nullptr,"Error",QString("File: %1 opend error!").arg(ConfigLoader::getInstance()->getSystemCSVPath()));
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
    this->lastItemWritePos = 0;
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
