#include "../include/ConfigLoader.h"

ConfigLoader* ConfigLoader::thePtr = new ConfigLoader();  //hungry mode
ConfigLoader::GbClear ConfigLoader::m_GbClear;

ConfigLoader::ConfigLoader(){
    this->clear();
    this->ConstructOperationPatten();
    qDebug()<<"Comming";
}

ConfigLoader::~ConfigLoader(){
    this->clear();
}

ConfigLoader* ConfigLoader::getInstance(){
    if(!thePtr){
        thePtr = new ConfigLoader();
    }

    return thePtr;
}

const std::map<int,QString>* ConfigLoader::getTheOperatorPatten() const{
    return this->theOperatorPatten;
}

void ConfigLoader::ConstructOperationPatten(){
    if(this->theOperatorPatten){
        std::map<int,QString>().swap(*this->theOperatorPatten);
        this->theOperatorPatten->clear();
        delete this->theOperatorPatten;
        this->theOperatorPatten = NULL;
    }

    this->theOperatorPatten = new std::map<int,QString>();
    this->theOperatorPatten->insert(std::pair<int,QString>(0,"PatentComeIn"));
    this->theOperatorPatten->insert(std::pair<int,QString>(1,"PatentImaging"));
    this->theOperatorPatten->insert(std::pair<int,QString>(2,"Theraphy"));
    this->theOperatorPatten->insert(std::pair<int,QString>(3,"LeavingRoom"));
}

void ConfigLoader::clear(){
    if(this->theJson){
        for(QJsonObject::iterator it = this->theJson->begin(); it != this->theJson->end(); it++){
            this->theJson->erase(it);
        }
        delete this->theJson;
        this->theJson = NULL;
    }

    if(this->theOperatorPatten){
        std::map<int,QString>().swap(*this->theOperatorPatten);
        this->theOperatorPatten->clear();
        delete this->theOperatorPatten;
        this->theOperatorPatten = NULL;
    }
}

/*Garbge clear*/
ConfigLoader::GbClear::GbClear(){

}

ConfigLoader::GbClear::~GbClear(){
    if(thePtr){
        delete thePtr;
        thePtr = NULL;
    }
}
