#ifndef CONFIG_CONFIGLOADER_H
#define CONFIG_CONFIGLOADER_H

#include <QJsonObject>
#include <map>
#include <string>
#include <QString>

static QString systemCSVPath = "TR.csv";
static QString systemDBPath = "TR.db";
static QString systemCfgPath = "TR.json";
static unsigned long totalShowCSVLine = 1000;

struct OneOperationPattern{
public:
    OneOperationPattern(const QString & label,
                        const QString & name,
                        signed int time):buttonLabel(label),buttonName(name),repeatTime(time){}

    OneOperationPattern(const std::string & label,
                        const std::string & name,
                        signed int time):buttonLabel(label.c_str()),buttonName(name.c_str()),repeatTime(time){}

    OneOperationPattern(const char* label,
                        const char* name,
                        signed int time):buttonLabel(label),buttonName(name),repeatTime(time){}
public:
    QString buttonLabel = "";
    QString buttonName = "";
    signed int repeatTime = 0;
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

private:
    bool readPatientInfoPatternFromFile();
    void writePatientInfoPatternToFile(const std::map<unsigned int,patientInfoPair> &);
    bool readOperationPatternFromFile();
    void writeOperationPatternToFile(const std::map<unsigned int,OneOperationPattern> &);
    void setDefaultPatientInfoPatten();
    void setDefaultOperationPatten();

private:
    std::map<unsigned int,patientInfoPair>* thePatientInfoPatten = NULL;
    std::map<unsigned int,OneOperationPattern>* theOperationPatten = NULL;

private:
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

#endif // CONFIG_CONFIGLOADER_H
