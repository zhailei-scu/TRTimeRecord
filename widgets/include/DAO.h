#ifndef DAO_H
#define DAO_H

#include <QtSql/QSqlDatabase>

/**/
class DAO{
private:
    DAO() = delete;
    DAO(const DAO &) = delete;
    const DAO & operator = (const DAO &) = delete;
public:

private:
    static DAO * thePtr;

public:
    DAO * getInstance();

private:
    QSqlDatabase *dataBase = nullptr;

private:
    class GbClear{
        void clear();
    };

    static GbClear m_GbClear;

};

#endif // DAO_H
