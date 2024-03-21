#ifndef COMMON_UTIL_JSONEXT_H
#define COMMON_UTIL_JSONEXT_H
#include <map>
#include <string>
#include <vector>

const std::string objectStartFlag = "{";
const std::string objectEndFlag = "}";
const std::string arrayStartFlag = "[";
const std::string arrayEndFlag = "]";
const std::string commaFlag = ",";
const std::string colonFlag = ":";

class JsonExt;

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
    const JsonBase* getJsonInfo() const;
    void Extract(const char* jsonFile);

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
