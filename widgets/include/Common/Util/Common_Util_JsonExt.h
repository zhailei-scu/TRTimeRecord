#ifndef COMMON_UTIL_JSONEXT_H
#define COMMON_UTIL_JSONEXT_H
#include <map>
#include <string>
#include <vector>
#include <ostream>
#include <fstream>
#include <iomanip>

class JsonExt;

class JsonFlags{
private:
    JsonFlags();
    virtual ~JsonFlags();
    JsonFlags(const JsonFlags &) = delete;
    const JsonFlags & operator =(const JsonFlags &) = delete;
private:
    static JsonFlags* m_JsonFlags;
public:
    static const JsonFlags* getInstance();

    std::string objectStartFlag = "{";
    std::string objectEndFlag = "}";
    std::string arrayStartFlag = "[";
    std::string arrayEndFlag = "]";
    std::string commaFlag = ",";
    std::string colonFlag = ":";
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

class JsonBase{
public:
    JsonBase();
    ~JsonBase();
    JsonBase(const JsonBase &);
    const JsonBase & operator = (const JsonBase &);

public:

    friend JsonExt;

    std::map<std::string,std::string> *namedPairs = NULL;
    std::map<std::string,JsonBase*> *namedObjects = NULL;
    std::map<int,JsonBase*> *objectsArray = NULL;

public:
    void writeBackToIo(std::ostream & io) const;
    void print() const;
    void reset();
};

class JsonExt{
public:
    JsonExt();
    ~JsonExt();
    JsonExt(const JsonExt &);
    const JsonExt & operator = (const JsonExt &);

public:
    JsonBase* getJsonInfo() const;
    void Extract(std::ifstream & ifs);
    void WriteBackToFile(const char* outFile,const std::ios::openmode & theMode);
private:
    JsonBase *theJsonInfo = NULL;

private:
    void clear();
    void EraseUselesschars(std::string & s);
    void Extract(std::vector<std::string> & clearInfo);
    void Extract(JsonBase* treeRoot,std::vector<std::string> & clearInfo);
    void Extract(JsonBase* base,std::vector<std::string> & clearInfo,int & pos,int & endPos);
    void SplitByFlag(std::vector<std::string> & clearInfo);
    void SplitByFlag(std::vector<std::string> & clearInfo,const char flag);
    void Check(const JsonBase* treeRoot);

private:
    friend JsonBase;
    static void Traverse(const JsonBase* treeRoot,std::vector<std::string> & traverseInfo);
};


#endif // COMMON_UTIL_JSONEXT_H
