#ifndef DAO_INTERFACE_H
#define DAO_INTERFACE_H

#include <QString>
#include <map>

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
};
#endif // DAO_INTERFACE_H
