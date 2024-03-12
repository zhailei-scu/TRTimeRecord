#ifndef CONFIGLOADER_H
#define CONFIGLOADER_H

#include <QJsonObject>
#include <map>

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
    const std::map<int,QString>* getTheOperatorPatten() const;

private:
    std::map<int,QString>* theOperatorPatten = NULL;

private:
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

#endif // CONFIGLOADER_H
