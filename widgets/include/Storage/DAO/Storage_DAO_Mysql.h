#ifndef STORAGE_DAO_MYSQL_H
#define STORAGE_DAO_MYSQL_H

#include <QSqlDatabase>
#include <list>
#include "Storage_DAO_Interface.h"
/**/
class DAO_Mysql: public DAO_Interface{
public:
    DAO_Mysql(QWidget* parent = NULL,const QString & linkName = "");
    DAO_Mysql(const DAO_Mysql &) = delete;
    const DAO_Mysql & operator = (const DAO_Mysql &) = delete;
    virtual ~DAO_Mysql();

/*Realize the interface*/
    virtual bool isDataBaseOpened();
    virtual bool tableExisted(const QString & tableName);
    virtual QString getRowCount(const QString & tableName);
    virtual void createEmptyTable_TR(const QString & tableName);
    virtual void createEmptyTable_Patient();
    virtual void appendARow_TR(const QString & tableName,
                               const std::map<unsigned int,std::pair<QString,QString>> & patientInfos,
                               const std::map<unsigned int,QString> & operatorTimes);
    virtual void updateARow_Patient(const std::map<unsigned int,std::pair<QString,QString>> & patientInfos,bool lock = true);
    virtual void updateARow_Patient(const std::map<QString,unsigned int> & colName,const bool lock = true);
    virtual void appendARow_Patient(const std::map<QString,unsigned int> & colName,const QString & values,bool lock = true);
    virtual void deleteLastRecord(const QString & tableName);
    virtual std::list<QString> getAllTablesName();
    virtual std::list<QString> getLikelyTablesName(const QString & tableName);
    virtual void updateTableName_TR(QString & str,
                                    const std::map<unsigned int,OnePatientPattern> & patientPattern,
                                    const std::map<unsigned int,OneOperationPattern> & OperationPattern);
    virtual bool needToUpdateTable_Patient(const std::map<unsigned int,OnePatientPattern> & patientPattern);
    virtual void updateTable_Patient();

    virtual void getOneColData_Patient(const QString & key,
                                       const QString & seperate,
                                       const QString & preAppendStr,
                                       const QString & postAppendStr,
                                       QStringList & result) const;
    virtual void getMultiColData_Patient(const QString & primaryKey,
                                         const std::map<QString,unsigned int> & columNames,
                                         const QString & seperate,
                                         const QString & preAppendStr,
                                         const QString & postAppendStr,
                                         std::map<QString,QString> & result) const;
    virtual void getMultiColData_Patient(const QString & primaryKey,
                                         const QString & columNames,
                                         const QString & seperate,
                                         const QString & preAppendStr,
                                         const QString & postAppendStr,
                                         std::map<QString,QString> & result) const;

    virtual void getRowValueByItemValue_Patient(const QString & key,
                                                const QString & value,
                                                const QString & seperate,
                                                const QString & preAppendStr,
                                                const QString & postAppendStr,
                                                std::map<QString,QString> & result) const;

    virtual bool columnExisted(const QString & tableName,const QString & colName) const;
    virtual void getAllColumnName(const QString & tableName,std::map<QString,unsigned int> & result) const;
private:
    virtual void generateSQL_appendARow_Patient(const std::map<unsigned int,std::pair<QString,QString>> & patientInfos,QString & result);
    virtual void generateSQL_appendARow_Patient(const std::map<QString,unsigned int> & colName,const QString & values,QString & result);
    virtual void generateSQL_appendARow_Patient(const QString & colName,const QString & values,QString & result);
    void clear();
};

#endif // STORAGE_DAO_MYSQL_H
