#include "../../include/Config/ConfigLoader.h"

ConfigLoader* ConfigLoader::thePtr = new ConfigLoader();  //hungry mode
ConfigLoader::GbClear ConfigLoader::m_GbClear;

ConfigLoader::ConfigLoader(){
    this->clear();

    this->ConstructOperationPatten();
    this->ConstructPatientInfoPatten();
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

const std::map<unsigned int,QString>* ConfigLoader::getTheOperatorPatten() const{
    return this->theOperatorPatten;
}

const std::map<unsigned int,patientInfoPair>* ConfigLoader::getThePatientInfoPatten() const{
    return this->thePatientInfoPatten;
}

void ConfigLoader::setThePatientPattern(const std::map<unsigned int,patientInfoPair> & patientPattern){


    this->writePatientInfoPatternToFile(patientPattern);
}

void ConfigLoader::ConstructOperationPatten(){
    if(this->theOperatorPatten){
        std::map<unsigned int,QString>().swap(*this->theOperatorPatten);
        this->theOperatorPatten->clear();
        delete this->theOperatorPatten;
        this->theOperatorPatten = NULL;
    }

    this->theOperatorPatten = new std::map<unsigned int,QString>();
    this->theOperatorPatten->insert(std::pair<unsigned int,QString>(0,"PatientComeIn"));
    this->theOperatorPatten->insert(std::pair<unsigned int,QString>(1,"PatentImaging"));
    this->theOperatorPatten->insert(std::pair<unsigned int,QString>(2,"Theraphy"));
    this->theOperatorPatten->insert(std::pair<unsigned int,QString>(3,"LeavingRoom"));
}

void ConfigLoader::ConstructPatientInfoPatten(){
    if(this->thePatientInfoPatten){
        std::map<unsigned int,patientInfoPair>().swap(*this->thePatientInfoPatten);
        this->thePatientInfoPatten->clear();
        delete this->thePatientInfoPatten;
        this->thePatientInfoPatten = NULL;
    }

    this->readPatientInfoPatternFromFile();
    this->thePatientInfoPatten = new std::map<unsigned int,patientInfoPair>();
    this->thePatientInfoPatten->insert(std::pair<unsigned int,patientInfoPair>(0,"PatientID"));
    this->thePatientInfoPatten->insert(std::pair<unsigned int,patientInfoPair>(1,"PatientName"));
    this->thePatientInfoPatten->insert(std::pair<unsigned int,patientInfoPair>(2,"TherapyOrgan"));
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
        std::map<unsigned int,QString>().swap(*this->theOperatorPatten);
        this->theOperatorPatten->clear();
        delete this->theOperatorPatten;
        this->theOperatorPatten = NULL;
    }

    if(this->thePatientInfoPatten){
        std::map<unsigned int,patientInfoPair>().swap(*this->thePatientInfoPatten);
        this->thePatientInfoPatten->clear();
        delete this->thePatientInfoPatten;
        this->thePatientInfoPatten = NULL;
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
