#ifndef DAO_H
#define DAO_H

#include <QSqlDatabase>
#include "DAO_Interface.h"
/**/
class DAO: public DAO_Interface{
private:
    DAO();
    DAO(const DAO &) = delete;
    const DAO & operator = (const DAO &) = delete;
    virtual ~DAO();
public:

private:
    static DAO * thePtr;

private:
    QSqlDatabase *theDataBase = NULL;

public:
    static DAO * getInstance();
    static void Start();

/*Realize the interface*/
    virtual bool tableExisted(const QString & tableName);
    virtual QString getRowCount(const QString & tableName);
    virtual void createEmptyTable(const QString & tableName);
    virtual void appendARow(const QString & tableName,
                            const std::map<unsigned int,QString> & patientInfos,
                            const std::map<unsigned int,QString> & operatorTimes);
    virtual void deleteLastRecord(const QString & tableName);

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

#endif // DAO_H