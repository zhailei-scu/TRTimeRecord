#ifndef GLOBAL_CONFIG_CONFIGLOADER_H
#define GLOBAL_CONFIG_CONFIGLOADER_H

#include <QJsonObject>
#include <map>
#include <string>
#include <QString>

static QString systemCSVPath = "TR.csv";
static QString systemDBPath = "TR.db";
static QString systemCfgPath = "TR.json";
static unsigned long totalShowCSVLine = 1000;


class OnlineInfoPattern{
private:
    OnlineInfoPattern();
    OnlineInfoPattern(const OnlineInfoPattern & ) = delete;
    const OnlineInfoPattern & operator = (const OnlineInfoPattern &) = delete;
    virtual ~OnlineInfoPattern();
private:
    static OnlineInfoPattern *m_ptr;

    std::map<QString,QString> defalutPattern;

public:
    static OnlineInfoPattern* getInstance();
    const std::map<QString,QString> & getDefalutPattern();

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

struct OneOperationPattern{
public:
    OneOperationPattern(){}

    OneOperationPattern(const QString & label,
                        const QString & name,
                        signed int time,
                        std::map<unsigned int,QString> & infos):buttonLabel(label),buttonName(name),repeatTime(time),hintInfos(infos){}

    OneOperationPattern(const std::string & label,
                        const std::string & name,
                        signed int time,
                        std::map<unsigned int,QString> & infos):buttonLabel(label.c_str()),buttonName(name.c_str()),repeatTime(time),hintInfos(infos){}

    OneOperationPattern(const char* label,
                        const char* name,
                        signed int time,
                        std::map<unsigned int,QString> & infos):buttonLabel(label),buttonName(name),repeatTime(time),hintInfos(infos){}
public:
    QString buttonLabel = "";
    QString buttonName = "";
    signed int repeatTime = 0;
    std::map<unsigned int,QString> hintInfos;

public:
    void clear(){
        buttonLabel = "";
        buttonName = "";
        repeatTime = 0;
        std::map<unsigned int,QString>().swap(hintInfos);
        hintInfos.clear();
    }
};

class map_value_finder_Operator
{
public:
    map_value_finder_Operator( const QString &cmp_value):m_value(cmp_value){}
    bool  operator ()(const std::pair<unsigned int,OneOperationPattern> &pair)
    {
        return pair.second.buttonName == m_value;
    }
private:
    const QString &m_value;
};

using patientInfoPair = std::pair<QString,QString>; //(label, Name)
/**/
class ConfigLoader{
private:
    ConfigLoader();
    virtual ~ConfigLoader();
    ConfigLoader(const ConfigLoader &) = delete;
    const ConfigLoader & operator = (const ConfigLoader &) = delete;

private:
    static ConfigLoader* thePtr;
    QJsonObject *theJson = NULL;

public:
    static ConfigLoader* getInstance();
    static void Start();
public:
    const std::map<unsigned int,patientInfoPair>* getThePatientInfoPatten() const;
    const std::map<unsigned int,OneOperationPattern>* getTheOperationPatten() const;
    void setThePatientPattern(const std::map<unsigned int,patientInfoPair> & patientPattern);
    void setTheOperationPattern(const std::map<unsigned int,OneOperationPattern> & operationPattern);
    const std::map<QString,QString> & getOnlineDatabaseInfo() const;
    void setOnlineDatabaseInfo(const std::map<QString,QString> & );

private:
    bool readOnlineDatabaseInfoFromFile();
    void writeOnlineDatabaseInfoToFile(const std::map<QString,QString> &);
    void setDefaultOnlineDatabaseInfo();
    bool readPatientInfoPatternFromFile();
    void writePatientInfoPatternToFile(const std::map<unsigned int,patientInfoPair> &);
    void setDefaultPatientInfoPatten();
    bool readOperationPatternFromFile();
    void writeOperationPatternToFile(const std::map<unsigned int,OneOperationPattern> &);
    void setDefaultOperationPatten();

private:
    std::map<unsigned int,patientInfoPair>* thePatientInfoPatten = NULL;
    std::map<unsigned int,OneOperationPattern>* theOperationPatten = NULL;
    std::map<QString,QString> onlineDBInfo;

private:
    void ConstructOnlineDBInfo();
    void ConstructPatientInfoPatten();
    void ConstructOperationPatten();
    void clear();

private:
    class GbClear{
    public:
        GbClear();
        ~GbClear();
    };

    static GbClear m_GbClear;
};

#endif // GLOBAL_CONFIG_CONFIGLOADER_H
