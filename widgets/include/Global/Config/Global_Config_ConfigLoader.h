#ifndef GLOBAL_CONFIG_CONFIGLOADER_H
#define GLOBAL_CONFIG_CONFIGLOADER_H

#include <QJsonObject>
#include <map>
#include <string>
#include <QString>

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

    std::map<unsigned int,QString> defalutPattern;

public:
    static OnlineInfoPattern* getInstance();
    const std::map<unsigned int,QString> & getDefalutPattern();

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

//using patientInfoPair = std::pair<QString,QString>; //(label, Name)

struct OnePatientPattern{
public:
    OnePatientPattern(){}

    OnePatientPattern(const QString & label,
                      const QString & name,
                      const QString & isNecessary,
                      const QString & isSupportQuery ="false",
                      bool isPrimaryKey = false,
                      bool isUnRemoveable = false):labelName(label),
                                                   infoName(name),
                                                   necessary(isNecessary),
                                                   supportPreQuery(isSupportQuery),
                                                   primaryKey(isPrimaryKey),
                                                   unRemoveable(isUnRemoveable){}

    OnePatientPattern(const std::string & label,
                      const std::string & name,
                      const std::string & isNecessary,
                      const std::string & isSupportQuery ="false",
                      bool isPrimaryKey = false,
                      bool isUnRemoveable = false):labelName(label.c_str()),
                                                   infoName(name.c_str()),
                                                   necessary(isNecessary.c_str()),
                                                   supportPreQuery(isSupportQuery.c_str()),
                                                   primaryKey(isPrimaryKey),
                                                   unRemoveable(isUnRemoveable){}

    OnePatientPattern(const char* label,
                      const char* name,
                      const char* isNecessary,
                      const char* isSupportQuery ="false",
                      bool isPrimaryKey = false,
                      bool isUnRemoveable = false):labelName(label),
                                                   infoName(name),
                                                   necessary(isNecessary),
                                                   supportPreQuery(isSupportQuery),
                                                   primaryKey(isPrimaryKey),
                                                   unRemoveable(isUnRemoveable){}

public:
    QString labelName = "";
    QString infoName = "";
    QString necessary = "false";
    QString supportPreQuery = "false";
    bool primaryKey = false;
    bool unRemoveable = false;

public:
    void clear(){
        labelName = "";
        infoName = "";
        necessary = "false";
    }
};

class map_value_finder_PatientInfos
{
public:
    map_value_finder_PatientInfos(const QString &cmp_value):m_value(cmp_value){}
    bool  operator ()(const std::pair<unsigned int,OnePatientPattern> &pair)
    {
        return pair.second.infoName == m_value;
    }
private:
    const QString &m_value;
};


/**/
class FixedPatientInfoPattern{
private:
    FixedPatientInfoPattern();
    virtual ~FixedPatientInfoPattern();
    FixedPatientInfoPattern(const FixedPatientInfoPattern &) = delete;
    const FixedPatientInfoPattern & operator = (const FixedPatientInfoPattern &) = delete;

private:
    static FixedPatientInfoPattern* m_ptr;
    std::map<unsigned int,OnePatientPattern> theFixedPatientInfoPatten;

public:
    static FixedPatientInfoPattern* getInstance();
    static const std::map<unsigned int,OnePatientPattern> & getTheFixedPatientInfoPatten();

private:
    void clear();

private:
    class GbClear{
    public:
        GbClear();
        virtual ~GbClear();
    };
    static GbClear m_GbClear;
};

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
    const std::map<unsigned int,OnePatientPattern>* getThePatientInfoPatten() const;
    const std::map<unsigned int,OneOperationPattern>* getTheOperationPatten() const;
    void setThePatientPattern(const std::map<unsigned int,OnePatientPattern> & patientPattern);
    void setTheOperationPattern(const std::map<unsigned int,OneOperationPattern> & operationPattern);
    const std::map<QString,QString> & getOnlineDatabaseInfo() const;
    void setOnlineDatabaseInfo(const std::map<QString,QString> & );
    void setSystemCSVPath(const QString &);
    const QString & getSystemCSVPath() const;
    bool readCSVStorageFromFile();
private:
    bool readOnlineDatabaseInfoFromFile();
    void writeOnlineDatabaseInfoToFile(const std::map<QString,QString> &);
    void setDefaultOnlineDatabaseInfo();
    bool readPatientInfoPatternFromFile();
    void writePatientInfoPatternToFile(const std::map<unsigned int,OnePatientPattern> &);
    void setDefaultPatientInfoPatten();
    bool readOperationPatternFromFile();
    void writeOperationPatternToFile(const std::map<unsigned int,OneOperationPattern> &);
    void setDefaultOperationPatten();

private:
    std::map<unsigned int,OnePatientPattern>* thePatientInfoPatten = NULL;
    std::map<unsigned int,OneOperationPattern>* theOperationPatten = NULL;
    std::map<QString,QString> onlineDBInfo;
    QString systemCSVPath = "TR.csv";
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
