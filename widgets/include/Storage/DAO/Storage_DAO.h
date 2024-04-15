#ifndef STORAGE_DAO_H
#define STORAGE_DAO_H

#include <QSqlDatabase>
#include "Storage_DAO_Interface.h"
#include "../../../include/Storage/DAO/Storage_DAO_Sqlite.h"
#include "../../../include/Storage/DAO/Storage_DAO_Mysql.h"

const QString patientInfo_linkName_Sqlite = "patientInfoConnect_Sqlite";
const QString patientInfo_linkName_MySql = "patientInfoConnect_MySql";
const QString trRecord_linkName = "trInfoConnection";

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
    DAO_Interface* patientInfoConnection = NULL;
    DAO_Interface* trInfoConnection = NULL;
public:
    static DAO * getInstance();
    DAO_Interface * getPatientInfoConnection();
    DAO_Interface * getTrInfoConnection();
    static void Start();
    void patientInfoReConnect();

private:
    void sync_PatientInfo();
    void DoSync_PatientInfo_BetweenReomteAndLocal(DAO_Mysql* remote,DAO_Sqlite* local);
    void MergeBasedOnFirst(QString & first,
                           const std::map<QString,QString> & localColumnNames_Ori,
                           const std::map<QString,QString> & localColumnNames,
                           ManualMark mark_Local,
                           const QString & second,
                           const std::map<QString,QString> & remoteColumnNames_Ori,
                           const std::map<QString,QString> & remoteColumnNames,
                           ManualMark mark_Remote);
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
