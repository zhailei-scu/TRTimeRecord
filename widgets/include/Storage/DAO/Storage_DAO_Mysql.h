#ifndef STORAGE_DAO_MYSQL_H
#define STORAGE_DAO_MYSQL_H

#include <QSqlDatabase>
#include <list>
#include "Storage_DAO_Interface.h"
/**/
class DAO_Mysql: public DAO_Interface{
private:
    DAO_Mysql();
    DAO_Mysql(const DAO_Mysql &) = delete;
    const DAO_Mysql & operator = (const DAO_Mysql &) = delete;
    virtual ~DAO_Mysql();
public:

private:
    static DAO_Mysql * thePtr;

private:
    QSqlDatabase *theDataBase = NULL;

public:
    static DAO_Mysql * getInstance();
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

#endif // STORAGE_DAO_MYSQL_H
