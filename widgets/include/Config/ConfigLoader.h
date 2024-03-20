#ifndef CONFIGLOADER_H
#define CONFIGLOADER_H

#include <QJsonObject>
#include <map>
#include <QString>

static QString systemCSVPath = "TR.csv";
static QString systemDBPath = "TR.db";
static QString systemCfgPath = "TR.json";
static unsigned long totalShowCSVLine = 1000;

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
    void Start();
    const std::map<unsigned int,QString>* getTheOperatorPatten() const;
    const std::map<unsigned int,patientInfoPair>* getThePatientInfoPatten() const;
    void setThePatientPattern(const std::map<unsigned int,patientInfoPair> & patientPattern);

private:
    std::map<unsigned int,QString>* theOperatorPatten = NULL;
    std::map<unsigned int,patientInfoPair>* thePatientInfoPatten = NULL;

private:
    void ConstructOperationPatten();
    void ConstructPatientInfoPatten();
    void clear();

private:
    class GbClear{
    public:
        GbClear();
        ~GbClear();
    };

    static GbClear m_GbClear;
};

#endif // CONFIGLOADER_H
