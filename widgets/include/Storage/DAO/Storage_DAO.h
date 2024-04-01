#ifndef STORAGE_DAO_H
#define STORAGE_DAO_H

#include <QSqlDatabase>
#include "Storage_DAO_Interface.h"

/**/
class DAO{
private:
    DAO();
    DAO(const DAO &) = delete;
    const DAO & operator = (const DAO &) = delete;
    virtual ~DAO();
public:

private:
    static DAO * thePtr;

private:
    DAO_Interface* connection = NULL;
public:
    static DAO * getInstance();
    DAO_Interface * getConnection();
    static void Start();
    void reConnect();

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

#endif // STORAGE_DAO_H
