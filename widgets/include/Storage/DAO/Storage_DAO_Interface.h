#ifndef STORAGE_DAO_INTERFACE_H
#define STORAGE_DAO_INTERFACE_H

#include <QString>
#include <map>
#include <list>

class DAO_Interface{
public:
    DAO_Interface(){};
    virtual ~DAO_Interface(){};

public:
    virtual bool tableExisted(const QString & tableName) = 0;
    virtual QString getRowCount(const QString & tableName) = 0;
    virtual void createEmptyTable(const QString & tableName) = 0;
    virtual void appendARow(const QString & tableName,
                            const std::map<unsigned int,QString> & patientInfos,
                            const std::map<unsigned int,QString> & operatorTimes) = 0;
    virtual void deleteLastRecord(const QString & tableName) = 0;
    virtual std::list<QString> getAllTablesName() = 0;
    virtual std::list<QString> getLikelyTablesName(const QString & tableName) = 0;
    virtual void updateTableName(QString & tableName,
                                 const std::map<unsigned int,QString> & patientInfoRecord,
                                 const std::map<unsigned int,QString> & buttonTimeRecord) = 0;
};
#endif // STORAGE_DAO_INTERFACE_H
