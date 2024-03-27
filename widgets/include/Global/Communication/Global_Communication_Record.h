#ifndef GLOBAL_COMMUNICATION_RECORD_H
#define GLOBAL_COMMUNICATION_RECORD_H
#include <QString>
#include <map>

class Record{
private:
    Record();
    virtual ~Record();
    Record(const Record &) = delete;
    const Record & operator = (const Record &) = delete;

private:
    static Record * m_theInstance;
public:
    std::map<unsigned int,QString> patientInfoRecord;
    std::map<unsigned int,QString> buttonTimeRecord;
    QString lastTableName;

public:
    static Record* getInstance();
    void clear();

private:
    class GbClear{
    public:
        GbClear();
        ~GbClear();
    };
    static GbClear m_GbClear;
};

#endif // GLOBAL_COMMUNICATION_RECORD_H
