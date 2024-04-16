#ifndef STORAGE_CSV_WRITER_H
#define STORAGE_CSV_WRITER_H

#include <QString>
#include <fstream>
#include <map>
/**/
class CSVWriter{
private:
    CSVWriter();
    CSVWriter(const CSVWriter &) = delete;
    const CSVWriter & operator = (const CSVWriter &) = delete;
    virtual ~CSVWriter();
public:

private:
    static CSVWriter * thePtr;
    std::ofstream ofs;
    QString lastLabel;
    long lastItemWritePos = 0;

public:
    static CSVWriter * getInstance();
    static void Start();

    void appendARecord(const QString & tableName,
                       const std::map<unsigned int,std::pair<QString,QString>> & patientInfos,
                       const std::map<unsigned int,QString> & operatorTimes);

    void deleteLastRecord();
    void reOpen();

    /*Realize the interface*/

private:
    void clear();
    QString findLastLabel();

private:
    class GbClear{
    public:
        GbClear();
        ~GbClear();
    };

    static GbClear m_GbClear;
};


#endif // STORAGE_CSV_WRITER_H
