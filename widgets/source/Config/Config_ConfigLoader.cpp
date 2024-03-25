#include "../../include/Config/Config_ConfigLoader.h"
#include "../../include/Common/Util/Common_Util_JsonExt.h"
#include <fstream>
#include <sstream>
#include <QMessageBox>

static std::string str_PatientInfoPattern = "PatientInfoPattern";
static std::string str_OperationPattern = "OperationPattern";

ConfigLoader* ConfigLoader::thePtr = NULL;  //lazy mode
ConfigLoader::GbClear ConfigLoader::m_GbClear;

ConfigLoader::ConfigLoader(){
    this->clear();

    this->ConstructPatientInfoPatten();
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

void ConfigLoader::Start(){
    getInstance();
}

const std::map<unsigned int,patientInfoPair>* ConfigLoader::getThePatientInfoPatten() const{
    return this->thePatientInfoPatten;
}

const std::map<unsigned int,OneOperationPattern>* ConfigLoader::getTheOperationPatten() const{
    return this->theOperationPatten;
}

void ConfigLoader::setThePatientPattern(const std::map<unsigned int,patientInfoPair> & patientPattern){
    if(this->thePatientInfoPatten){
        std::map<unsigned int,patientInfoPair>().swap(*this->thePatientInfoPatten);
        this->thePatientInfoPatten->clear();
        delete this->thePatientInfoPatten;
        this->thePatientInfoPatten = NULL;
    }

    this->thePatientInfoPatten = new std::map<unsigned int,patientInfoPair>();

    for(std::map<unsigned int,patientInfoPair>::const_iterator it = patientPattern.begin();
                                                               it != patientPattern.end();
                                                               it++){
        this->thePatientInfoPatten->insert(std::pair<unsigned int,patientInfoPair>(*it));
    }

    this->writePatientInfoPatternToFile(*this->thePatientInfoPatten);
}


void ConfigLoader::setTheOperationPattern(const std::map<unsigned int,OneOperationPattern> & operationPattern){
    if(this->theOperationPatten){
        std::map<unsigned int,OneOperationPattern>().swap(*this->theOperationPatten);
        this->theOperationPatten->clear();
        delete this->theOperationPatten;
        this->theOperationPatten = NULL;
    }

    this->theOperationPatten = new std::map<unsigned int,OneOperationPattern>();

    for(std::map<unsigned int,OneOperationPattern>::const_iterator it = operationPattern.begin();
                                                                   it != operationPattern.end();
                                                                   it++){
        this->theOperationPatten->insert(*it);
    }

    this->writeOperationPatternToFile(*this->theOperationPatten);
}

bool ConfigLoader::readPatientInfoPatternFromFile(){
    std::stringstream ss;
    int id = 0;
    bool result = false;
    std::ifstream ifs;
    JsonExt* ext = new JsonExt();

    this->thePatientInfoPatten = new std::map<unsigned int,patientInfoPair>();

    ifs.open(systemCfgPath.toStdString().c_str());

    if(ifs.is_open()){
        ext->Extract(ifs);

        if(ext->getJsonInfo() && ext->getJsonInfo()->namedObjects){
            std::map<std::string,JsonBase*>::const_iterator it_top = ext->getJsonInfo()->namedObjects->find(str_PatientInfoPattern);
            if(it_top != ext->getJsonInfo()->namedObjects->end()){
                if(it_top->second && it_top->second->namedObjects){
                    for(std::map<std::string,JsonBase*>::iterator it = it_top->second->namedObjects->begin();
                                                                  it != it_top->second->namedObjects->end();
                                                                  it++){
                        if(it->second->namedPairs){
                            ss.str("");
                            ss.clear();
                            ss<<it->first;
                            ss>>id;

                            if(0 == it->second->namedPairs->size()){
                                QMessageBox::critical(nullptr,
                                                      "Error",
                                                      QString("The json file for patient input id: %1 is empty").arg(id));
                                exit(-1);
                            }

                            if(1 < it->second->namedPairs->size()){
                                QMessageBox::critical(nullptr,
                                                      "Error",
                                                      QString("The json file for patient input id: %1 included %2 information")
                                                          .arg(id).arg(it->second->namedPairs->size()));
                                exit(-1);
                            }

                            if(this->thePatientInfoPatten->find(id) == this->thePatientInfoPatten->end()){
                                std::string labelName = it->second->namedPairs->begin()->first;
                                std::string name = it->second->namedPairs->begin()->second;
                                name.erase(0,name.find_first_not_of(" "));
                                name.erase(name.find_last_not_of(" ")+1);
                                if((signed int)(name.find_first_of(" "))>0){
                                    QMessageBox::critical(nullptr,
                                                          "Error",
                                                          QString("The json file for patient input id: %1 included %2 blank")
                                                              .arg(id).arg(name.c_str()));
                                    exit(-1);
                                }

                                this->thePatientInfoPatten->insert(std::pair<unsigned int,patientInfoPair>
                                                                   (id,patientInfoPair(QString(labelName.c_str()),QString(name.c_str()))));
                                result = true;
                            }else{
                                QMessageBox::critical(nullptr,
                                                      "Error",
                                                      QString("The json file for patient input id: %1 repeated").arg(id));
                                exit(-1);
                            }
                        }

                    }

                }
            }
        }

        ifs.close();
    }

    delete ext;
    ext = NULL;

    if(!result){
        if(this->thePatientInfoPatten){
            std::map<unsigned int,patientInfoPair>().swap(*this->thePatientInfoPatten);
            delete this->thePatientInfoPatten;
            this->thePatientInfoPatten = NULL;
        }
    }

    return result;
}

void ConfigLoader::writePatientInfoPatternToFile(const std::map<unsigned int,patientInfoPair> & input){
    std::ifstream ifs;
    std::stringstream ss;
    std::string str_Index;
    JsonExt* ext = new JsonExt();
    JsonBase* base = NULL;
    JsonBase* patientPattern = NULL;
    JsonBase* onePatientInfoLine = NULL;

    ifs.open(systemCfgPath.toStdString());
    if(ifs.is_open()){
        ext->Extract(ifs);
    }
    ifs.close();

    base = ext->getJsonInfo();
    if(base){
        if(ext->getJsonInfo()->namedObjects){
            for(std::map<std::string,JsonBase*>::iterator it = ext->getJsonInfo()->namedObjects->begin();
                                                          it != ext->getJsonInfo()->namedObjects->end();
                                                          it++){
                if(str_PatientInfoPattern == it->first){
                    //remove
                    ext->getJsonInfo()->namedObjects->erase(it);
                }
            }
        }
    }else{
        base = new JsonBase();
        ext->setJsonInfo(base);
    }

    if(!base->namedObjects){
        base->namedObjects = new std::map<std::string,JsonBase*>();
    }

    patientPattern = new JsonBase();

    patientPattern->namedObjects = new std::map<std::string,JsonBase*>();

    for(std::map<unsigned int,patientInfoPair>::const_iterator it = input.begin();
                                                               it != input.end();
                                                               it++){
        ss.str("");
        ss.clear();
        ss<<it->first;
        str_Index = "";
        ss>>str_Index;
        onePatientInfoLine = new JsonBase();
        onePatientInfoLine->namedPairs = new std::map<std::string,std::string>();
        onePatientInfoLine->namedPairs->insert(std::pair<std::string,std::string>(it->second.first.toStdString(),it->second.second.toStdString()));

        patientPattern->namedObjects->insert(std::pair<std::string, JsonBase*>(str_Index,onePatientInfoLine));
    }

    base->namedObjects->insert(std::pair<std::string,JsonBase*>(str_PatientInfoPattern,patientPattern));

    ext->WriteBackToFile(systemCfgPath.toStdString().c_str(),std::ios::ate);

    delete ext;
    ext = NULL;
}

bool ConfigLoader::readOperationPatternFromFile(){
    std::stringstream ss;
    std::string buttonLabel;
    std::string OperationName;
    int id = 0;
    signed int repeatTimes = 0;
    bool result = false;
    std::ifstream ifs;
    JsonBase * onePair = NULL;
    JsonExt* ext = new JsonExt();

    this->theOperationPatten = new std::map<unsigned int,OneOperationPattern>();

    ifs.open(systemCfgPath.toStdString().c_str());

    if(ifs.is_open()){
        ext->Extract(ifs);

        if(ext->getJsonInfo() && ext->getJsonInfo()->namedObjects){
            std::map<std::string,JsonBase*>::const_iterator it_top = ext->getJsonInfo()->namedObjects->find(str_OperationPattern);
            if(it_top != ext->getJsonInfo()->namedObjects->end()){
                if(it_top->second && it_top->second->namedObjects){
                    for(std::map<std::string,JsonBase*>::iterator it = it_top->second->namedObjects->begin();
                         it != it_top->second->namedObjects->end();
                         it++){
                        if(it->second->namedObjects){
                            ss.str("");
                            ss.clear();
                            ss<<it->first;
                            ss>>id;

                            if(0 == it->second->namedObjects->size()){
                                QMessageBox::critical(nullptr,
                                                      "Error",
                                                      QString("The json file for operation pipeline input id: %1 is empty").arg(id));
                                exit(-1);
                            }

                            if(1 < it->second->namedObjects->size()){
                                QMessageBox::critical(nullptr,
                                                      "Error",
                                                      QString("The json file for operation pipeline input id: %1 included %2 information")
                                                          .arg(id).arg(it->second->namedObjects->size()));
                                exit(-1);
                            }

                            if(this->theOperationPatten->find(id) == this->theOperationPatten->end()){
                                buttonLabel = it->second->namedObjects->begin()->first;

                                onePair = it->second->namedObjects->begin()->second;

                                if(!onePair || !onePair->namedPairs || 0 == onePair->namedPairs->size()){
                                    QMessageBox::critical(nullptr,
                                                          "Error",
                                                          QString("The pipeline button label %1 not include any operationName and repeat timess")
                                                              .arg(buttonLabel.c_str()));
                                    exit(-1);
                                }

                                if(1 < onePair->namedPairs->size()){
                                    QMessageBox::critical(nullptr,
                                                          "Error",
                                                          QString("The pipeline button label %1 include too much pairs of operationName and repeat timess")
                                                              .arg(buttonLabel.c_str()));
                                    exit(-1);
                                }

                                OperationName = onePair->namedPairs->begin()->first;

                                OperationName.erase(0,OperationName.find_first_not_of(" "));
                                OperationName.erase(OperationName.find_last_not_of(" ")+1);
                                if((signed int)(OperationName.find_first_of(" "))>0){
                                    QMessageBox::critical(nullptr,
                                                          "Error",
                                                          QString("The json file for operation pipeline input id: %1 included %2 blank")
                                                              .arg(id).arg(OperationName.c_str()));
                                    exit(-1);
                                }

                                ss.str("");
                                ss.clear();
                                ss<<onePair->namedPairs->begin()->second;
                                ss>>repeatTimes;
                                if(0 == repeatTimes || (repeatTimes < 0 && -1 != repeatTimes)){
                                    QMessageBox::critical(nullptr,
                                                          "Error",
                                                          QString("The json file for operation pipeline input id: %1 included wrong repeat time %2:"
                                                                  "The repeatTime can only be -1(represent nolimitation) or positive integer value")
                                                              .arg(id).arg(repeatTimes));
                                    exit(-1);
                                }

                                this->theOperationPatten->insert(std::pair<unsigned int,OneOperationPattern>
                                                                 (id,OneOperationPattern(QString(buttonLabel.c_str()),QString(OperationName.c_str()),repeatTimes)));
                                result = true;
                            }else{
                                QMessageBox::critical(nullptr,
                                                      "Error",
                                                      QString("The json file for operation pipeline input id: %1 repeated").arg(id));
                                exit(-1);
                            }
                        }

                    }

                }
            }
        }

        ifs.close();
    }

    delete ext;
    ext = NULL;

    if(!result){
        if(this->theOperationPatten){
            std::map<unsigned int,OneOperationPattern>().swap(*this->theOperationPatten);
            delete this->theOperationPatten;
            this->theOperationPatten = NULL;
        }
    }

    return result;
}

void ConfigLoader::writeOperationPatternToFile(const std::map<unsigned int,OneOperationPattern> & input){
    std::ifstream ifs;
    std::stringstream ss;
    std::string str_Index;
    std::string str_Repeat;
    JsonExt* ext = new JsonExt();
    JsonBase* base = NULL;
    JsonBase* operationPattern = NULL;
    JsonBase* oneOperation = NULL;

    ifs.open(systemCfgPath.toStdString());
    if(ifs.is_open()){
        ext->Extract(ifs);
    }
    ifs.close();

    base = ext->getJsonInfo();
    if(base){
        if(ext->getJsonInfo()->namedObjects){
            for(std::map<std::string,JsonBase*>::iterator it = ext->getJsonInfo()->namedObjects->begin();
                 it != ext->getJsonInfo()->namedObjects->end();
                 it++){
                if(str_OperationPattern == it->first){
                    //remove
                    ext->getJsonInfo()->namedObjects->erase(it);
                }
            }
        }
    }else{
        base = new JsonBase();
        ext->setJsonInfo(base);
    }

    if(!base->namedObjects){
        base->namedObjects = new std::map<std::string,JsonBase*>();
    }

    operationPattern = new JsonBase();

    operationPattern->namedObjects = new std::map<std::string,JsonBase*>();

    for(std::map<unsigned int,OneOperationPattern>::const_iterator it = input.begin();
                                                                   it != input.end();
                                                                   it++){
        ss.str("");
        ss.clear();
        ss<<it->first;
        str_Index = "";
        ss>>str_Index;

        ss.str("");
        ss.clear();
        ss<<it->second.repeatTime;
        str_Repeat = "";
        ss>>str_Repeat;

        oneOperation = new JsonBase();
        oneOperation->namedObjects = new std::map<std::string,JsonBase*>();
        JsonBase *onePair = new JsonBase();
        onePair->namedPairs = new std::map<std::string,std::string>();
        onePair->namedPairs->insert(std::pair<std::string,std::string>(it->second.buttonName.toStdString(),str_Repeat));

        oneOperation->namedObjects->insert(std::pair<std::string,JsonBase*>(it->second.buttonLabel.toStdString(),onePair));

        operationPattern->namedObjects->insert(std::pair<std::string, JsonBase*>(str_Index,oneOperation));
    }

    base->namedObjects->insert(std::pair<std::string,JsonBase*>(str_OperationPattern,operationPattern));

    ext->WriteBackToFile(systemCfgPath.toStdString().c_str(),std::ios::ate);

    delete ext;
    ext = NULL;
}

void ConfigLoader::ConstructPatientInfoPatten(){
    if(this->thePatientInfoPatten){
        std::map<unsigned int,patientInfoPair>().swap(*this->thePatientInfoPatten);
        this->thePatientInfoPatten->clear();
        delete this->thePatientInfoPatten;
        this->thePatientInfoPatten = NULL;
    }

    bool result = this->readPatientInfoPatternFromFile();
    if(!result){
        this->setDefaultPatientInfoPatten();
        /*
        this->thePatientInfoPatten = new std::map<unsigned int,patientInfoPair>();
        this->thePatientInfoPatten->insert(std::pair<unsigned int,patientInfoPair>(0,"PatientID"));
        this->thePatientInfoPatten->insert(std::pair<unsigned int,patientInfoPair>(1,"PatientName"));
        this->thePatientInfoPatten->insert(std::pair<unsigned int,patientInfoPair>(2,"TherapyOrgan"));
        */
        this->writePatientInfoPatternToFile(*this->thePatientInfoPatten);
    }
}

void ConfigLoader::ConstructOperationPatten(){
    if(this->theOperationPatten){
        std::map<unsigned int,OneOperationPattern>().swap(*this->theOperationPatten);
        this->theOperationPatten->clear();
        delete this->theOperationPatten;
        this->theOperationPatten = NULL;
    }

    bool result = this->readOperationPatternFromFile();
    if(!result){
        this->setDefaultOperationPatten();
        this->writeOperationPatternToFile(*this->theOperationPatten);
    }
}

void ConfigLoader::setDefaultPatientInfoPatten(){
    if(this->thePatientInfoPatten){
        std::map<unsigned int,patientInfoPair>().swap(*this->thePatientInfoPatten);
        this->thePatientInfoPatten->clear();
        delete this->thePatientInfoPatten;
        this->thePatientInfoPatten = NULL;
    }
    this->thePatientInfoPatten = new std::map<unsigned int,patientInfoPair>();
    this->thePatientInfoPatten->insert(std::pair<unsigned int,patientInfoPair>(0,patientInfoPair("PatientID","PatientID")));
    this->thePatientInfoPatten->insert(std::pair<unsigned int,patientInfoPair>(1,patientInfoPair("PatientName","PatientName")));
    this->thePatientInfoPatten->insert(std::pair<unsigned int,patientInfoPair>(2,patientInfoPair("TherapyOrgan","TherapyOrgan")));
}

void ConfigLoader::setDefaultOperationPatten(){
    if(this->theOperationPatten){
        std::map<unsigned int,OneOperationPattern>().swap(*this->theOperationPatten);
        this->theOperationPatten->clear();
        delete this->theOperationPatten;
        this->theOperationPatten = NULL;
    }

    this->theOperationPatten = new std::map<unsigned int,OneOperationPattern>();
    this->theOperationPatten->insert(std::pair<unsigned int,OneOperationPattern>(0,OneOperationPattern("Patient come in","PatientComeIn",1)));
    this->theOperationPatten->insert(std::pair<unsigned int,OneOperationPattern>(1,OneOperationPattern("Imaging/Position","PatentImaging",2)));
    this->theOperationPatten->insert(std::pair<unsigned int,OneOperationPattern>(2,OneOperationPattern("Therapy","Theraphy",-1)));
    this->theOperationPatten->insert(std::pair<unsigned int,OneOperationPattern>(3,OneOperationPattern("Leaving Room","LeavingRoom",1)));
}

void ConfigLoader::clear(){
    if(this->theJson){
        for(QJsonObject::iterator it = this->theJson->begin(); it != this->theJson->end(); it++){
            this->theJson->erase(it);
        }
        delete this->theJson;
        this->theJson = NULL;
    }

    if(this->thePatientInfoPatten){
        std::map<unsigned int,patientInfoPair>().swap(*this->thePatientInfoPatten);
        this->thePatientInfoPatten->clear();
        delete this->thePatientInfoPatten;
        this->thePatientInfoPatten = NULL;
    }

    if(this->theOperationPatten){
        std::map<unsigned int,OneOperationPattern>().swap(*this->theOperationPatten);
        this->theOperationPatten->clear();
        delete this->theOperationPatten;
        this->theOperationPatten = NULL;
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
