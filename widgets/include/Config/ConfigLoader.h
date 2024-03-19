#ifndef CONFIGLOADER_H
#define CONFIGLOADER_H

#include <QJsonObject>
#include <map>

static QString systemCSVPath = "TR.csv";
static QString systemDBPath = "TR.db";
static unsigned long totalShowCSVLine = 1000;

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
    const std::map<unsigned int,QString>* getTheOperatorPatten() const;
    const std::map<unsigned int,QString>* getThePatientInfoPatten() const;

private:
    std::map<unsigned int,QString>* theOperatorPatten = NULL;
    std::map<unsigned int,QString>* thePatientInfoPatten = NULL;

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
