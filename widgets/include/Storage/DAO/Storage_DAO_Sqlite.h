#ifndef STORAGE_DAO_SQLITE_H
#define STORAGE_DAO_SQLITE_H

#include <QSqlDatabase>
#include <list>
#include "Storage_DAO_Interface.h"
/**/

class DAO_Sqlite: public DAO_Interface{
public:
    DAO_Sqlite(QWidget* parent = NULL,const QString & linkName = "");
    DAO_Sqlite(const DAO_Sqlite &) = delete;
    const DAO_Sqlite & operator = (const DAO_Sqlite &) = delete;
    virtual ~DAO_Sqlite();

public:
/*Realize the interface*/
    virtual bool isDataBaseOpened();
    virtual bool tableExisted(const QString & tableName);
    virtual QString getRowCount(const QString & tableName);
    virtual void createEmptyTable_TR(const QString & tableName);
    virtual void createEmptyTable_Patient();
    virtual void appendARow_TR(const QString & tableName,
                               const std::map<unsigned int,QString> & patientInfos,
                               const std::map<unsigned int,QString> & operatorTimes);
    virtual void appendARow_Patient(const std::map<unsigned int,QString> & patientInfos);
    virtual void deleteLastRecord(const QString & tableName);
    virtual std::list<QString> getAllTablesName();
    virtual std::list<QString> getLikelyTablesName(const QString & tableName);
    virtual void updateTableName_TR(QString & str,
                                    const std::map<unsigned int,OnePatientPattern> & patientPattern,
                                    const std::map<unsigned int,OneOperationPattern> & OperationPattern);
    virtual bool needToUpdateTable_Patient(const std::map<unsigned int,OnePatientPattern> & patientPattern);
    virtual void updateTable_Patient();

private:
    void clear();
};

#endif // STORAGE_DAO_SQLITE_H
