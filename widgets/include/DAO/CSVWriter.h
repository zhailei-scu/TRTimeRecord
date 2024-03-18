#ifndef CSVWRITER_H
#define CSVWRITER_H

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

public:
    static CSVWriter * getInstance();
    static void Start();

    void appendARecord(const QString & tableName,
                       const std::map<unsigned int,QString> & patientInfos,
                       const std::map<unsigned int,QString> & operatorTimes);

    void deleteLastRecord(const QString & tableName,
                       const std::map<unsigned int,QString> & patientInfos,
                       const std::map<unsigned int,QString> & operatorTimes);

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


#endif // CSVWRITER_H
