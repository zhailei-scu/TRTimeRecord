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
                               const std::map<unsigned int,QString> & patientInfos,
                               const std::map<unsigned int,QString> & operatorTimes);
    virtual void appendARow_Patient(const std::map<unsigned int,QString> & patientInfos);
    virtual void deleteLastRecord(const QString & tableName);
    virtual std::list<QString> getAllTablesName();
    virtual std::list<QString> getLikelyTablesName(const QString & tableName);
    virtual void updateTableName_TR(QString & str,
                                    const std::map<unsigned int,patientInfoPair> & patientPattern,
                                    const std::map<unsigned int,OneOperationPattern> & OperationPattern);
    virtual void updateTable_Patient(const std::map<unsigned int,patientInfoPair> & patientPattern);

private:
    void clear();
};

#endif // STORAGE_DAO_MYSQL_H
