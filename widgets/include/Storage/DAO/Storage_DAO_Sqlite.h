#ifndef STORAGE_DAO_SQLITE_H
#define STORAGE_DAO_SQLITE_H

#include <QSqlDatabase>
#include <list>
#include "Storage_DAO_Interface.h"
/**/
class DAO_Sqlite: public DAO_Interface{
private:
    DAO_Sqlite();
    DAO_Sqlite(const DAO_Sqlite &) = delete;
    const DAO_Sqlite & operator = (const DAO_Sqlite &) = delete;
    virtual ~DAO_Sqlite();
public:

private:
    static DAO_Sqlite * thePtr;

private:
    QSqlDatabase *theDataBase = NULL;

public:
    static DAO_Sqlite * getInstance();
    static void Start();

/*Realize the interface*/
    virtual bool tableExisted(const QString & tableName);
    virtual QString getRowCount(const QString & tableName);
    virtual void createEmptyTable(const QString & tableName);
    virtual void appendARow(const QString & tableName,
                            const std::map<unsigned int,QString> & patientInfos,
                            const std::map<unsigned int,QString> & operatorTimes);
    virtual void deleteLastRecord(const QString & tableName);
    virtual std::list<QString> getAllTablesName();
    virtual std::list<QString> getLikelyTablesName(const QString & tableName);
    virtual void updateTableName(QString & str,
                                 const std::map<unsigned int,patientInfoPair> & patientPattern,
                                 const std::map<unsigned int,OneOperationPattern> & OperationPattern);

private:
    void clear();

private:
    class GbClear{
    public:
        GbClear();
        ~GbClear();
    };

    static GbClear m_GbClear;
};

#endif // STORAGE_DAO_SQLITE_H
