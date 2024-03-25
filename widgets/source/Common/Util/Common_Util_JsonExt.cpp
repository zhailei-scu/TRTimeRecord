#include "../../../include/Common/Util/Common_Util_JsonExt.h"
#include "../../../include/Common/Util/Common_Util_Base.h"
#include <QMessageBox>
#include <iostream>
#include <fstream>

/*JsonFlags*/
JsonFlags* JsonFlags::m_JsonFlags = NULL;
JsonFlags::GbClear JsonFlags::m_GbClear;

JsonFlags::JsonFlags(){
    this->clear();
}

JsonFlags::~JsonFlags(){
    this->clear();
}

const JsonFlags* JsonFlags::getInstance(){
    if(!m_JsonFlags){
        m_JsonFlags = new JsonFlags();
    }

    return m_JsonFlags;
}

void JsonFlags::clear(){
    objectStartFlag = "{";
    objectEndFlag = "}";
    arrayStartFlag = "[";
    arrayEndFlag = "]";
    commaFlag = ",";
    colonFlag = ":";
}

JsonFlags::GbClear::GbClear(){
}

JsonFlags::GbClear::~GbClear(){
    if(m_JsonFlags){
        delete m_JsonFlags;
        m_JsonFlags = NULL;
    }
}

/*JsonBase*/
JsonBase::JsonBase(){
    this->reset();
}

JsonBase::~JsonBase(){
    this->reset();
}

JsonBase::JsonBase(const JsonBase & subject){
    *this = subject;
}

const JsonBase & JsonBase::operator=(const JsonBase & subject){
    this->reset();

    if(subject.namedPairs){
        this->namedPairs = new std::map<std::string,std::string>();
        *this->namedPairs = *subject.namedPairs;
    }

    if(subject.namedObjects){
        this->namedObjects = new std::map<std::string,JsonBase*>();
        for(std::map<std::string,JsonBase*>::iterator it = subject.namedObjects->begin();
             it != subject.namedObjects->end();
             it++){
            JsonBase* temp = new JsonBase();
            *temp = *it->second;
            this->namedObjects->insert(std::pair<std::string,JsonBase*>(it->first,temp));
        }
    }

    if(subject.objectsArray){
        this->objectsArray = new std::map<int,JsonBase*>();
        for(std::map<int,JsonBase*>::iterator it = subject.objectsArray->begin();
             it != subject.objectsArray->end();
             it++){
            JsonBase* temp = new JsonBase();
            *temp = *it->second;
            this->objectsArray->insert(std::pair<int,JsonBase*>(it->first,temp));
        }

    }

    return *this;
}

void JsonBase::reset(){
    if(this->namedPairs){
        std::map<std::string,std::string>().swap(*this->namedPairs);
        delete this->namedPairs;
        this->namedPairs = NULL;
    }

    if(this->namedObjects){
        std::map<std::string,JsonBase*>().swap(*this->namedObjects);
        delete this->namedObjects;
        this->namedObjects = NULL;
    }

    if(this->objectsArray){
        std::map<int,JsonBase*>().swap(*this->objectsArray);
        delete this->objectsArray;
        this->objectsArray = NULL;
    }
}

void JsonBase::writeBackToIo(std::ostream & io) const{
    std::vector<std::string> info;
    JsonExt::Traverse(this,info);
    for(std::vector<std::string>::iterator it = info.begin();it!=info.end();it++){
        io<<*it<<std::endl;
    }
}

void JsonBase::print() const{
    this->writeBackToIo(std::cout);
}

//---------------JsonExt-----------
JsonExt::JsonExt(){
    this->clear();
}

JsonExt::~JsonExt(){
    this->clear();
}

JsonExt::JsonExt(const JsonExt & subject){
    *this = subject;
}

const JsonExt & JsonExt::operator=(const JsonExt & subject){
    this->clear();
    if(subject.theJsonInfo){
        this->theJsonInfo = new JsonBase();
        *this->theJsonInfo = *subject.theJsonInfo;
    }

    return *this;
}

void JsonExt::WriteBackToFile(const char* outFile,const std::ios::openmode & theMode = std::ios::ate){
    std::ofstream ofs;
    std::vector<std::string> info;

    if(this->theJsonInfo){
        ofs.open(outFile,theMode);

        if(ofs.is_open()){
            this->theJsonInfo->writeBackToIo(ofs);

            ofs.close();
        }else{
            QMessageBox::critical(nullptr,"Error",QString("The file is not finded %1 !").arg(outFile));
            exit(-1);
        }
    }
}

void JsonExt::Extract(std::ifstream & ifs){
    std::string line;
    std::vector<std::string> clearInfo;  //remove the comments
    while(getline(ifs,line)){
        this->EraseUselesschars(line);

        if(line.size() >0){
            clearInfo.push_back(line);
        }
    }

    this->Extract(clearInfo);
}

void JsonExt::Extract(std::vector<std::string> & clearInfo){
    if(!this->theJsonInfo) this->theJsonInfo = new JsonBase();
    this->Extract(this->theJsonInfo,clearInfo);
    this->Check(this->theJsonInfo);
}

void JsonExt::Extract(JsonBase* treeRoot,std::vector<std::string> & clearInfo){
    this->SplitByFlag(clearInfo);

    //erease the ","
    clearInfo.erase(std::remove(clearInfo.begin(),clearInfo.end(),","),std::end(clearInfo));

    int pos = 0;
    int endPos = clearInfo.size();

    this->Extract(this->theJsonInfo,clearInfo,pos,endPos);
}

void JsonExt::Extract(JsonBase* base,
                      std::vector<std::string> & clearInfo,
                      int & pos,
                      int & endPos){

    std::string tempStrSub;
    std::string tempStrObj;

    while(pos<endPos){

        if(!base){
            QMessageBox::critical(nullptr,"Error",QString("Internal Error: The object is not initialized"
                                                          "When parse the json file at flatted line: %1").arg(pos));
            exit(-1);
        }

        pos++;

        if(JsonFlags::getInstance()->objectStartFlag == clearInfo.at(pos)){
            if(':' != clearInfo.at(pos-1).back()){ // the array {[{

                if(JsonFlags::getInstance()->arrayStartFlag != clearInfo.at(pos-1) && base->objectsArray->size() <=0){
                    QMessageBox::critical(nullptr,"Error",QString("You must special the array flag at flatted line: %1").arg(pos));
                    exit(-1);
                }

                int index = base->objectsArray->size();
                JsonBase *temp = new JsonBase();
                base->objectsArray->insert(std::pair<int,JsonBase*>(index,temp));
                this->Extract(temp,clearInfo,pos,endPos);

            }else{ //named objects {"":{
                if(JsonFlags::getInstance()->objectStartFlag == clearInfo.at(pos-2) ||
                    JsonFlags::getInstance()->objectEndFlag == clearInfo.at(pos-2)   ||
                    JsonFlags::getInstance()->arrayStartFlag == clearInfo.at(pos-2)  ||
                    JsonFlags::getInstance()->arrayStartFlag == clearInfo.at(pos-2)){
                    QMessageBox::critical(nullptr,
                                          "Error",
                                          QString("The key of named object cannot be any flag: %1 "
                                                  "When parse the json file at flatted line: %2").arg(clearInfo.at(pos-1).c_str())
                                                                                                 .arg(pos));
                    exit(-1);
                }

                if(!base->namedObjects) base->namedObjects = new std::map<std::string,JsonBase*>();

                if(base->namedObjects->end() != base->namedObjects->find(clearInfo.at(pos-2))){
                    QMessageBox::critical(nullptr,"Error",QString("The key was redefined: %1").arg(clearInfo.at(pos-2).c_str()));
                    exit(-1);
                }

                tempStrSub = clearInfo.at(pos-2);
                StringOperation::EraseHeadAndTail(tempStrSub,'\"');
                //transform(tempStrSub.begin(),tempStrSub.end(),tempStrSub.begin(),::tolower);
                JsonBase *temp = new JsonBase();
                base->namedObjects->insert(std::pair<std::string,JsonBase*>(tempStrSub,temp));
                this->Extract(temp,clearInfo,pos,endPos);
            }

        }else if(JsonFlags::getInstance()->arrayStartFlag == clearInfo.at(pos)){ //array
            if(!base->objectsArray) base->objectsArray = new std::map<int,JsonBase*>();
        }else if(JsonFlags::getInstance()->objectEndFlag == clearInfo.at(pos)){
            return;
        }else{

            if(':' == clearInfo.at(pos-1).back()){ // the pair

                if(JsonFlags::getInstance()->objectStartFlag == clearInfo.at(pos-2) ||
                   JsonFlags::getInstance()->objectEndFlag == clearInfo.at(pos-2)   ||
                   JsonFlags::getInstance()->arrayStartFlag == clearInfo.at(pos-2)  ||
                   JsonFlags::getInstance()->arrayStartFlag == clearInfo.at(pos-2)){
                   QMessageBox::critical(nullptr,
                                         "Error",
                                         QString("The key of named object cannot be any flag: %1 "
                                                 "When parse the json file at flatted line: %2").arg(clearInfo.at(pos-1).c_str())
                                                                                                .arg(pos));
                    exit(-1);
                }

                if(!base->namedPairs) base->namedPairs = new std::map<std::string,std::string>();

                if(base->namedPairs->end() != base->namedPairs->find(clearInfo.at(pos-2))){
                    QMessageBox::critical(nullptr,"Error",QString("The key was redefined: %1").arg(clearInfo.at(pos-2).c_str()));
                    exit(-1);
                }
                tempStrSub = clearInfo.at(pos-2);
                StringOperation::EraseHeadAndTail(tempStrSub,'\"');
                //transform(tempStrSub.begin(),tempStrSub.end(),tempStrSub.begin(),::tolower);
                tempStrObj = clearInfo.at(pos);
                StringOperation::EraseHeadAndTail(tempStrObj,'\"');

                base->namedPairs->insert(std::pair<std::string,std::string>(tempStrSub,tempStrObj));
            }

        }
    }
}

void JsonExt::Traverse(const JsonBase* treeRoot,std::vector<std::string> & traverseInfo,int blankNum){
    if(!treeRoot){
        return;
    }

    std::string blank="";

    for(int i = 0;i<blankNum;i++){
        blank.append(" ");
    }

    traverseInfo.push_back(blank + JsonFlags::getInstance()->objectStartFlag);

    if(treeRoot->namedPairs){

        std::map<std::string,std::string>::iterator itNamed = treeRoot->namedPairs->begin();
        while(itNamed !=  treeRoot->namedPairs->end()){
            traverseInfo.push_back(blank + itNamed->first + ":" + itNamed->second);
            itNamed++;
        }
    }

    if(treeRoot->objectsArray){
        traverseInfo.push_back(blank + JsonFlags::getInstance()->arrayStartFlag);
        std::map<int,JsonBase*>::iterator itArray = treeRoot->objectsArray->begin();
        while(itArray !=  treeRoot->objectsArray->end()){
            Traverse(const_cast<const JsonBase*>(itArray->second),traverseInfo,(blankNum+1)*2);
            itArray++;
        }
        traverseInfo.push_back(blank + JsonFlags::getInstance()->arrayEndFlag);
    }

    if(treeRoot->namedObjects){
        std::map<std::string,JsonBase*>::iterator itNamed = treeRoot->namedObjects->begin();
        while(itNamed !=  treeRoot->namedObjects->end()){
            traverseInfo.push_back(blank + itNamed->first + ":");
            Traverse(const_cast<const JsonBase*>(itNamed->second),traverseInfo,(blankNum+1)*2);

            itNamed++;
        }
    }

    traverseInfo.push_back(blank + JsonFlags::getInstance()->objectEndFlag);
}

void JsonExt::Check(const JsonBase* treeRoot){
    std::vector<std::string> CheckInfo_Before;
    this->Traverse(this->theJsonInfo,CheckInfo_Before);

    qDebug()<<"Parsed json file:";
    for(std::vector<std::string>::iterator it = CheckInfo_Before.begin();it!=CheckInfo_Before.end();it++){
        qDebug()<<*it;
    }

    /*
  vector<string> CheckInfo_Before;
  vector<string> CheckInfo_After;
  vector<string> CheckInfo_Compare;
  JsonBase* CheckingJson;

  std::cout<<"Checking json file extracting..."<<std::endl;
  this->Traverse(this->theJsonInfo,CheckInfo_Before);

  CheckInfo_After = CheckInfo_Before;

  CheckingJson = new JsonBase();
  this->Extract(CheckingJson,CheckInfo_After);

  this->Traverse(this->theJsonInfo,CheckInfo_Compare);

  if(CheckInfo_Compare != CheckInfo_Before){
    std::cout<<"Json file extract field!"<<std::endl;
  }

  delete CheckingJson;
  vector<string>().swap(CheckInfo_Before);
  vector<string>().swap(CheckInfo_After);
  vector<string>().swap(CheckInfo_Compare);
  */
}

void JsonExt::SplitByFlag(std::vector<std::string> & clearInfo){
    this->SplitByFlag(clearInfo,JsonFlags::getInstance()->objectStartFlag.at(0));
    this->SplitByFlag(clearInfo,JsonFlags::getInstance()->objectEndFlag.at(0));
    this->SplitByFlag(clearInfo,JsonFlags::getInstance()->arrayStartFlag.at(0));
    this->SplitByFlag(clearInfo,JsonFlags::getInstance()->arrayEndFlag.at(0));
    this->SplitByFlag(clearInfo,JsonFlags::getInstance()->commaFlag.at(0));
    this->SplitByFlag(clearInfo,JsonFlags::getInstance()->colonFlag.at(0));
}

void JsonExt::SplitByFlag(std::vector<std::string> & clearInfo,const char flag){
    std::vector<std::string>::iterator it = clearInfo.begin();
    std::vector<std::string> tempSplitResult;
    std::vector<std::string> result;

    while(it != clearInfo.end()){
        StringOperation::split_includeFlag(it->c_str(),flag,tempSplitResult);

        for(std::vector<std::string>::iterator it=tempSplitResult.begin();it != tempSplitResult.end();it++){
            this->EraseUselesschars(*it);
            result.push_back(*it);
        }
        it++;
    }

    result.swap(clearInfo);
}

void JsonExt::EraseUselesschars(std::string & line){
    int commitPos = line.find_first_of("#");
    if(commitPos>=0)
        line.erase(commitPos);
    for(int i=0;i<=32;i++){
        StringOperation::EraseHeadAndTail(line,char(i));
    }
}

JsonBase* JsonExt::getJsonInfo() const{
    return this->theJsonInfo;
}

void JsonExt::setJsonInfo(JsonBase* info){
    this->theJsonInfo = info;
}

void JsonExt::clear(){
    if(this->theJsonInfo){
        delete this->theJsonInfo;
        theJsonInfo = NULL;
    }
}
