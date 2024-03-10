#ifndef DAO_H
#define DAO_H

#include <QSqlDatabase>

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
    QSqlDatabase *theDataBase = NULL;

public:
    static DAO * getInstance();
    static void Start();

private:
    QSqlDatabase *dataBase = nullptr;

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
