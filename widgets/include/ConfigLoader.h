#ifndef CONFIGLOADER_H
#define CONFIGLOADER_H

#include <QJsonObject>

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

#endif // CONFIGLOADER_H
