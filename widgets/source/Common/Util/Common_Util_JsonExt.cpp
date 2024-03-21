#include "../../../include/Common/Util/Common_Util_JsonExt.h"
#include <QMessageBox>
#include <fstream>

using namespace std;

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
        map<string,string>().swap(*this->namedPairs);
        delete this->namedPairs;
        this->namedPairs = NULL;
    }

    if(this->namedObjects){
        map<string,JsonBase*>().swap(*this->namedObjects);
        delete this->namedObjects;
        this->namedObjects = NULL;
    }

    if(this->objectsArray){
        map<int,JsonBase*>().swap(*this->objectsArray);
        delete this->objectsArray;
        this->objectsArray = NULL;
    }
}

void JsonBase::print()const{
    vector<string> info;
    JsonExt::Traverse(this,info);

    qDebug()<<"Parsed json file:";
    for(vector<string>::iterator it = info.begin();it!=info.end();it++){
        qDebug()<<*it;
    }
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

void JsonExt::Extract(const char* jsonFile){
    ifstream fs;
    string line;
    vector<string> clearInfo;  //remove the comments

    fs.open(jsonFile);

    if(fs.is_open()){

        while(getline(fs,line)){
            this->EraseUselesschars(line);

            if(line.size() >0){
                clearInfo.push_back(line);
            }
        }

        this->Extract(clearInfo);

        fs.close();

    }else{
        QMessageBox::critical(nullptr,"Error",QString("Error: open file : %1 failed").arg(jsonFile));
        exit(-1);
    }
}

void JsonExt::Extract(vector<string> & clearInfo){
    if(!this->theJsonInfo) this->theJsonInfo = new JsonBase();
    this->Extract(this->theJsonInfo,clearInfo);
    this->Check(this->theJsonInfo);
}

void JsonExt::Extract(JsonBase* treeRoot,vector<string> & clearInfo){
    this->SplitByFlag(clearInfo);

    //erease the ","
    clearInfo.erase(std::remove(clearInfo.begin(),clearInfo.end(),","),std::end(clearInfo));

    int pos = 0;
    int endPos = clearInfo.size();

    this->Extract(this->theJsonInfo,clearInfo,pos,endPos);
}

void JsonExt::Extract(JsonBase* base,
                      vector<string> & clearInfo,
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

        if(objectStartFlag == clearInfo.at(pos)){
            if(':' != clearInfo.at(pos-1).back()){ // the array {[{

                if(arrayStartFlag != clearInfo.at(pos-1) && base->objectsArray->size() <=0){
                    QMessageBox::critical(nullptr,"Error",QString("You must special the array flag at flatted line: %1").arg(pos));
                    exit(-1);
                }

                int index = base->objectsArray->size();
                JsonBase *temp = new JsonBase();
                base->objectsArray->insert(pair<int,JsonBase*>(index,temp));
                this->Extract(temp,clearInfo,pos,endPos);

            }else{ //named objects {"":{
                if(objectStartFlag == clearInfo.at(pos-2) ||
                    objectEndFlag == clearInfo.at(pos-2)   ||
                    arrayStartFlag == clearInfo.at(pos-2)  ||
                    arrayStartFlag == clearInfo.at(pos-2)){
                    QMessageBox::critical(nullptr,
                                          "Error",
                                          QString("The key of named object cannot be any flag: %1 "
                                                  "When parse the json file at flatted line: %2").arg(clearInfo.at(pos-1).c_str())
                                                                                                 .arg(pos));
                    exit(-1);
                }

                if(!base->namedObjects) base->namedObjects = new map<string,JsonBase*>();

                if(base->namedObjects->end() != base->namedObjects->find(clearInfo.at(pos-2))){
                    QMessageBox::critical(nullptr,"Error",QString("The key was redefined: %1").arg(clearInfo.at(pos-2).c_str()));
                    exit(-1);
                }

                tempStrSub = clearInfo.at(pos-2);
                StringOperation::EraseHeadAndTail(tempStrSub,'\"');
                transform(tempStrSub.begin(),tempStrSub.end(),tempStrSub.begin(),::tolower);
                JsonBase *temp = new JsonBase();
                base->namedObjects->insert(pair<string,JsonBase*>(tempStrSub,temp));
                this->Extract(temp,clearInfo,pos,endPos);
            }

        }else if(arrayStartFlag == clearInfo.at(pos)){ //array
            if(!base->objectsArray) base->objectsArray = new map<int,JsonBase*>();
        }else if(objectEndFlag == clearInfo.at(pos)){
            return;
        }else{

            if(':' == clearInfo.at(pos-1).back()){ // the pair

                if(objectStartFlag == clearInfo.at(pos-2) ||
                    objectEndFlag == clearInfo.at(pos-2)   ||
                    arrayStartFlag == clearInfo.at(pos-2)  ||
                    arrayStartFlag == clearInfo.at(pos-2)){
                    QMessageBox()

                    cout<<"Internal Error: The key of named object cannot be any flag"<<endl;
                    cout<<clearInfo.at(pos-1)<<endl;
                    cout<<"When parse the json file"<<endl;
                    cout<<"at flatted line: "<<pos<<endl;
                    exit(-1);
                }

                if(!base->namedPairs) base->namedPairs = new map<string,string>();

                if(base->namedPairs->end() != base->namedPairs->find(clearInfo.at(pos-2))){
                    std::cout<<"Error: the key was redefined: "<<clearInfo.at(pos-2)<<std::endl;
                    exit(-1);
                }
                tempStrSub = clearInfo.at(pos-2);
                StringOperation::EraseHeadAndTail(tempStrSub,'\"');
                transform(tempStrSub.begin(),tempStrSub.end(),tempStrSub.begin(),::tolower);
                tempStrObj = clearInfo.at(pos);
                StringOperation::EraseHeadAndTail(tempStrObj,'\"');

                base->namedPairs->insert(pair<string,string>(tempStrSub,tempStrObj));
            }

        }
    }
}

void JsonExt::Traverse(const JsonBase* treeRoot,vector<string> & traverseInfo){
    if(!treeRoot){
        return;
    }

    traverseInfo.push_back(objectStartFlag);

    if(treeRoot->namedPairs){

        map<string,string>::iterator itNamed = treeRoot->namedPairs->begin();
        while(itNamed !=  treeRoot->namedPairs->end()){
            traverseInfo.push_back(itNamed->first);
            traverseInfo.push_back(":");
            traverseInfo.push_back(itNamed->second);
            itNamed++;
        }
    }

    if(treeRoot->objectsArray){
        traverseInfo.push_back(arrayStartFlag);
        map<int,JsonBase*>::iterator itArray = treeRoot->objectsArray->begin();
        while(itArray !=  treeRoot->objectsArray->end()){
            Traverse(const_cast<const JsonBase*>(itArray->second),traverseInfo);
            itArray++;
        }
        traverseInfo.push_back(arrayEndFlag);
    }

    if(treeRoot->namedObjects){
        map<string,JsonBase*>::iterator itNamed = treeRoot->namedObjects->begin();
        while(itNamed !=  treeRoot->namedObjects->end()){
            traverseInfo.push_back(itNamed->first);
            traverseInfo.push_back(":");
            Traverse(const_cast<const JsonBase*>(itNamed->second),traverseInfo);

            itNamed++;
        }
    }

    traverseInfo.push_back(objectEndFlag);
}

void JsonExt::Check(const JsonBase* treeRoot){
    vector<string> CheckInfo_Before;
    this->Traverse(this->theJsonInfo,CheckInfo_Before);

    std::cout<<"Parsed json file:"<<std::endl;
    for(vector<string>::iterator it = CheckInfo_Before.begin();it!=CheckInfo_Before.end();it++){
        std::cout<<*it<<std::endl;
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

void JsonExt::SplitByFlag(vector<string> & clearInfo){
    this->SplitByFlag(clearInfo,objectStartFlag.at(0));
    this->SplitByFlag(clearInfo,objectEndFlag.at(0));
    this->SplitByFlag(clearInfo,arrayStartFlag.at(0));
    this->SplitByFlag(clearInfo,arrayEndFlag.at(0));
    this->SplitByFlag(clearInfo,commaFlag.at(0));
    this->SplitByFlag(clearInfo,colonFlag.at(0));
}

void JsonExt::SplitByFlag(vector<string> & clearInfo,const char flag){
    vector<string>::iterator it = clearInfo.begin();
    vector<string> tempSplitResult;
    vector<string> result;

    while(it != clearInfo.end()){
        StringOperation::split_includeFlag(it->c_str(),flag,tempSplitResult);

        for(vector<string>::iterator it=tempSplitResult.begin();it != tempSplitResult.end();it++){
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

const JsonBase* JsonExt::getJsonInfo() const{
    return this->theJsonInfo;
}

void JsonExt::clear(){
    if(this->theJsonInfo){
        delete this->theJsonInfo;
        theJsonInfo = NULL;
    }
}
