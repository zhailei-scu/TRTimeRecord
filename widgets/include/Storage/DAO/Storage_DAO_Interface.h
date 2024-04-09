#ifndef STORAGE_DAO_INTERFACE_H
#define STORAGE_DAO_INTERFACE_H

#include <QString>
#include <map>
#include <list>
#include <QSqlDatabase>
#include <QDialog>
#include "../../Global/Config/Global_Config_ConfigLoader.h"

const char appendFlag = 'F';
const QString patientInfo_TableName = "patientInfo";

class DAO_Interface:public QDialog{

public:
    DAO_Interface(QWidget* parent = NULL,const QString & Name = ""):QDialog(parent),linkName(Name){};
    virtual ~DAO_Interface(){};

protected:
    QSqlDatabase *theDataBase = NULL;
    QString linkName = "";

public:
    virtual bool isDataBaseOpened() = 0;
    virtual bool tableExisted(const QString & tableName) = 0;
    virtual QString getRowCount(const QString & tableName) = 0;
    virtual void createEmptyTable_TR(const QString & tableName) = 0;
    virtual void createEmptyTable_Patient() = 0;
    virtual void appendARow_TR(const QString & tableName,
                               const std::map<unsigned int,std::pair<QString,QString>> & patientInfos,
                               const std::map<unsigned int,QString> & operatorTimes) = 0;
    virtual void appendARow_Patient(const std::map<unsigned int,std::pair<QString,QString>> & patientInfos) = 0;
    virtual void deleteLastRecord(const QString & tableName) = 0;
    virtual std::list<QString> getAllTablesName() = 0;
    virtual std::list<QString> getLikelyTablesName(const QString & tableName) = 0;
    virtual void updateTableName_TR(QString & str,
                                    const std::map<unsigned int,OnePatientPattern> & patientPattern,
                                    const std::map<unsigned int,OneOperationPattern> & OperationPattern) = 0;
    virtual bool needToUpdateTable_Patient(const std::map<unsigned int,OnePatientPattern> & patientPattern) = 0;
    virtual void updateTable_Patient() = 0;
    virtual const QSqlDatabase* getTheDataBase() const{
        return this->theDataBase;
    }

    virtual const QString & getTheLinkName() const{
        return this->linkName;
    }

    virtual void getAllValueByKey_Patient(const QString & key,QStringList & result) const = 0;
    virtual void getRowValueByItemValue_Patient(const QString & key,const QString & value,std::map<QString,QString> & result) const = 0;

    virtual bool columnExisted(const QString & tableName,const QString & colName) const = 0;
    virtual void getAllColumnName(const QString & tableName,std::map<QString,unsigned int> & result) const = 0;
    virtual void getAllData_Patient(const QString & primaryKey,const std::map<QString,unsigned int> & columNames) const = 0;
};
#endif // STORAGE_DAO_INTERFACE_H
