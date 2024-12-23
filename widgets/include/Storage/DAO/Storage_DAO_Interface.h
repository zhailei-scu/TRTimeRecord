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
const QString patientInfo_ManualMark_TableName = "patientInfo_ManualMark";

enum ManualMark{
    ProActive = 0,
    Passive = 1
};

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
    virtual void createEmptyTable_Patient_ManualMark() = 0;
    virtual void appendARow_TR(const QString & tableName,
                               const std::map<unsigned int,std::pair<QString,QString>> & patientInfos,
                               const std::map<unsigned int,QString> & operatorTimes) = 0;
    virtual void updateARow_Patient(const std::map<unsigned int,std::pair<QString,QString>> & patientInfos,
                                    ManualMark mark,
                                    bool lock) = 0;
    virtual void updateARow_Patient(const QString & primaryKey,
                                    const QString & primaryValue,
                                    const std::map<QString,QString> & colName,
                                    const QString & values,
                                    ManualMark mark,
                                    bool lock) = 0;
    virtual void appendARow_Patient(const QString & primaryKey,
                                    const QString & primaryValue,
                                    const std::map<QString,QString> & colName,
                                    const QString & values,
                                    ManualMark mark,
                                    bool lock) = 0;
    virtual void updateARow_PatientManualMark(const QString & primaryKey,
                                              const QString & primaryKeyValue,
                                              ManualMark mark) = 0;
    virtual void insertACol_Patient(const QString & colName,
                                    const QString & colType,
                                    bool lock) = 0;
    virtual void deleteLastRecord_TR(const QString & tableName) = 0;
    virtual std::list<QString> getAllTablesName() = 0;
    virtual std::list<QString> getLikelyTablesName(const QString & tableName) = 0;
    virtual void updateTableName_TR(QString & str,
                                    const std::map<unsigned int,OnePatientPattern> & patientPattern,
                                    const std::map<unsigned int,OneOperationPattern> & OperationPattern) = 0;
    virtual bool needToUpdateTable_Patient(const std::map<unsigned int,OnePatientPattern> & patientPattern) = 0;
    virtual void updateTable_Patient(bool lock) = 0;
    virtual const QSqlDatabase* getTheDataBase() const{
        return this->theDataBase;
    }

    virtual const QString & getTheLinkName() const{
        return this->linkName;
    }

    virtual void getOneColData_Patient(const QString & key,
                                       const QString & seperate,
                                       const QString & preAppendStr,
                                       const QString & postAppendStr,
                                       QStringList & result) const = 0;
    virtual void getMultiColData_Patient(const QString & primaryKey,
                                         const std::map<QString,QString> & columNames,
                                         const QString & seperate,
                                         const QString & preAppendStr,
                                         const QString & postAppendStr,
                                         std::map<QString,QString> & result) const = 0;
    virtual void getMultiColData_Patient(const QString & primaryKey,
                                         const QString & columNames,
                                         const QString & seperate,
                                         const QString & preAppendStr,
                                         const QString & postAppendStr,
                                         std::map<QString,QString> & result) const = 0;

    virtual void getManualMark(const QString & primaryKey,std::map<QString,QString> & result) const = 0;

    virtual void getRowValueByItemValue_Patient(const QString & key,
                                                const QString & value,
                                                const QString & seperate,
                                                const QString & preAppendStr,
                                                const QString & postAppendStr,
                                                std::map<QString,QString> & result) const = 0;

    virtual bool columnExisted(const QString & tableName,const QString & colName) const = 0;
    virtual void getAllColumnName(const QString & tableName,std::map<QString,QString> & result) const = 0;

    virtual void generateSQL_appendARow_Patient(const std::map<unsigned int,std::pair<QString,QString>> & patientInfos,QString & result) = 0;
    virtual void generateSQL_appendARow_Patient(const std::map<QString,QString> & colName,const QString & values,QString & result)  = 0;
    virtual void generateSQL_appendARow_Patient(const QString & colName,const QString & values,QString & result)  = 0;
};
#endif // STORAGE_DAO_INTERFACE_H
