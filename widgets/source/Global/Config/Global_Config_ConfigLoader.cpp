#include "../../../include/Global/Config/Global_Config_ConfigLoader.h"
#include "../../../include/Common/Util/Common_Util_JsonExt.h"
#include "../../../include/Common/Util/Common_Util_Base.h"
#include <fstream>
#include <sstream>
#include <QMessageBox>

static std::string str_OnlineDatabaseInfo = "OnlineDatabaseInfo";
static std::string str_PatientInfoPattern = "PatientInfoPattern";
static std::string str_OperationPattern = "OperationPattern";

static std::string str_OperationLabel = "OperationLabel";
static std::string str_OperationName = "OperationName";
static std::string str_RepeatTimes = "RepeatTimes";
static std::string str_HintInfos = "HintInfos";

static std::string str_PatientInfoLabel = "PatientInfoLabel";
static std::string str_PatientInfoName = "PatientInfoName";
static std::string str_PatientInfoNecessary = "Necessary";
static std::string str_PatientInfoUnRemoveable = "UnRemoveable";

ConfigLoader* ConfigLoader::thePtr = NULL;  //lazy mode
ConfigLoader::GbClear ConfigLoader::m_GbClear;

FixedPatientInfoPattern* FixedPatientInfoPattern::m_ptr = NULL;  //lazy mode
FixedPatientInfoPattern::GbClear FixedPatientInfoPattern::m_GbClear;

OnlineInfoPattern* OnlineInfoPattern::m_ptr = NULL;
OnlineInfoPattern::GbClear OnlineInfoPattern::m_GbClear;

OnlineInfoPattern::OnlineInfoPattern(){
    this->clear();

    this->defalutPattern.insert(std::pair<unsigned int,QString>(0,"onlineDBIP"));
    this->defalutPattern.insert(std::pair<unsigned int,QString>(1,"onlineDBPort"));
    this->defalutPattern.insert(std::pair<unsigned int,QString>(2,"onlineDBName"));
    this->defalutPattern.insert(std::pair<unsigned int,QString>(3,"onlineDBUserName"));
    this->defalutPattern.insert(std::pair<unsigned int,QString>(4,"onlineDBPassword"));
}

OnlineInfoPattern::~OnlineInfoPattern(){
    this->clear();
}

OnlineInfoPattern* OnlineInfoPattern::getInstance(){
    if(!m_ptr){
        m_ptr = new OnlineInfoPattern();
    }

    return m_ptr;
}

const std::map<unsigned int,QString> & OnlineInfoPattern::getDefalutPattern(){
    return this->defalutPattern;
}

void OnlineInfoPattern::clear(){
    std::map<unsigned int,QString>().swap(this->defalutPattern);
    this->defalutPattern.clear();
}

OnlineInfoPattern::GbClear::GbClear(){

}

OnlineInfoPattern::GbClear::~GbClear(){
    if(m_ptr){
        delete m_ptr;
        m_ptr = NULL;
    }
}

/**/
FixedPatientInfoPattern::FixedPatientInfoPattern(){
    this->clear();
}

FixedPatientInfoPattern::~FixedPatientInfoPattern(){
    this->clear();
}

FixedPatientInfoPattern* FixedPatientInfoPattern::getInstance(){
    if(!m_ptr){
        m_ptr = new FixedPatientInfoPattern();
    }
    return m_ptr;
}

const std::map<unsigned int,OnePatientPattern> & FixedPatientInfoPattern::getTheFixedPatientInfoPatten(){
    return getInstance()->theFixedPatientInfoPatten;
}

void FixedPatientInfoPattern::clear(){
    std::map<unsigned int,OnePatientPattern>().swap(this->theFixedPatientInfoPatten);
    this->theFixedPatientInfoPatten.clear();

    OnePatientPattern tempPattern;
    tempPattern.labelName="UUID";
    tempPattern.infoName="UUID";
    tempPattern.necessary="true";
    tempPattern.primaryKey = true;
    tempPattern.unRemoveable = true;
    this->theFixedPatientInfoPatten.insert(std::pair<unsigned int,OnePatientPattern>(0,tempPattern));
}

FixedPatientInfoPattern::GbClear::GbClear(){

}

FixedPatientInfoPattern::GbClear::~GbClear(){
    if(m_ptr){
        delete m_ptr;
    }
    m_ptr = NULL;
}

ConfigLoader::ConfigLoader(){
    this->clear();

    this->ConstructPatientInfoPatten();
    this->ConstructOperationPatten();
    this->ConstructOnlineDBInfo();

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

const std::map<unsigned int,OnePatientPattern>* ConfigLoader::getThePatientInfoPatten() const{
    return this->thePatientInfoPatten;
}

const std::map<unsigned int,OneOperationPattern>* ConfigLoader::getTheOperationPatten() const{
    return this->theOperationPatten;
}

const std::map<QString,QString> & ConfigLoader::getOnlineDatabaseInfo() const{
    return this->onlineDBInfo;
}


void ConfigLoader::setOnlineDatabaseInfo(const std::map<QString,QString> & info){
    std::map<QString,QString>().swap(this->onlineDBInfo);
    this->onlineDBInfo.clear();
    this->onlineDBInfo = info;

    this->writeOnlineDatabaseInfoToFile(this->onlineDBInfo);
}

void ConfigLoader::setThePatientPattern(const std::map<unsigned int,OnePatientPattern> & patientPattern){
    if(this->thePatientInfoPatten){
        std::map<unsigned int,OnePatientPattern>().swap(*this->thePatientInfoPatten);
        this->thePatientInfoPatten->clear();
        delete this->thePatientInfoPatten;
        this->thePatientInfoPatten = NULL;
    }

    this->thePatientInfoPatten = new std::map<unsigned int,OnePatientPattern>();

    for(std::map<unsigned int,OnePatientPattern>::const_iterator it = patientPattern.begin();
                                                                 it != patientPattern.end();
                                                                 it++){
        this->thePatientInfoPatten->insert(std::pair<unsigned int,OnePatientPattern>(*it));
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

bool ConfigLoader::readOnlineDatabaseInfoFromFile(){
    std::stringstream ss;
    bool result = false;
    std::ifstream ifs;
    JsonExt* ext = new JsonExt();

    ifs.open(systemCfgPath.toStdString().c_str());

    if(ifs.is_open()){
        ext->Extract(ifs);

        if(ext->getJsonInfo() && ext->getJsonInfo()->namedObjects){
            std::map<std::string,JsonBase*>::const_iterator it_top = ext->getJsonInfo()->namedObjects->find(str_OnlineDatabaseInfo);
            if(it_top != ext->getJsonInfo()->namedObjects->end()){
                if(it_top->second && it_top->second->namedPairs){
                    result = true;
                    for(std::map<unsigned int,QString>::const_iterator it = OnlineInfoPattern::getInstance()->getDefalutPattern().begin();
                                                                       it != OnlineInfoPattern::getInstance()->getDefalutPattern().end();
                                                                       it++){
                        std::map<std::string,std::string>::const_iterator findIt = it_top->second->namedPairs->find(it->second.toStdString());
                        if(it_top->second->namedPairs->end() != findIt){
                            if(0 == this->onlineDBInfo.count(it->second)){
                                this->onlineDBInfo.insert(std::pair<QString,QString>(it->second,findIt->second.c_str()));
                            }else{
                                this->onlineDBInfo[it->second] = findIt->second.c_str();
                            }
                        }else{
                            if(0 == this->onlineDBInfo.count(it->second)){
                                this->onlineDBInfo.insert(std::pair<QString,QString>(it->second,""));
                            }else{
                                this->onlineDBInfo[it->second] = "";
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

    return result;
}

void ConfigLoader::writeOnlineDatabaseInfoToFile(const std::map<QString,QString> & input){
    std::ifstream ifs;
    JsonExt* ext = new JsonExt();
    JsonBase* base = NULL;
    JsonBase* databaseJson = NULL;

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
                if(str_OnlineDatabaseInfo == it->first){
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

    databaseJson = new JsonBase();

    databaseJson->namedPairs = new std::map<std::string,std::string>();

    for(std::map<QString,QString>::const_iterator it = input.begin();it != input.end();it++){
        databaseJson->namedPairs->insert(std::pair<std::string,std::string>(it->first.toStdString(),it->second.toStdString()));
    }

    base->namedObjects->insert(std::pair<std::string,JsonBase*>(str_OnlineDatabaseInfo,databaseJson));

    ext->WriteBackToFile(systemCfgPath.toStdString().c_str(),std::ios::ate);

    delete ext;
    ext = NULL;
}

bool ConfigLoader::readPatientInfoPatternFromFile(){
    std::stringstream ss;
    int id = 0;
    bool result = false;
    std::ifstream ifs;
    JsonExt* ext = new JsonExt();
    std::string labelName = "";
    std::string name = "";
    std::string necessary = "false";
    unsigned int idShift = 0;
    bool unmremoveable = false;

    this->thePatientInfoPatten = new std::map<unsigned int,OnePatientPattern>();

    /*Set unremoveable pattern*/
    for(auto it = FixedPatientInfoPattern::getInstance()->getTheFixedPatientInfoPatten().begin();
             it != FixedPatientInfoPattern::getInstance()->getTheFixedPatientInfoPatten().end();
             it++){
        this->thePatientInfoPatten->insert(*it);
    }
    idShift = FixedPatientInfoPattern::getInstance()->getTheFixedPatientInfoPatten().size();

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

                            if(this->thePatientInfoPatten->find(id+idShift) == this->thePatientInfoPatten->end()){
                                std::map<std::string,std::string>::const_iterator it_Info = it->second->namedPairs->find(str_PatientInfoLabel);
                                labelName = "";
                                if(it_Info != it->second->namedPairs->end()){
                                    labelName = it_Info->second;
                                }else{
                                    QMessageBox::critical(nullptr,
                                                          "Error",
                                                          QString("You must special the pair with label string: %1").arg(str_PatientInfoLabel.c_str()));
                                    exit(-1);
                                }

                                it_Info = it->second->namedPairs->find(str_PatientInfoName);
                                name = "";
                                if(it_Info != it->second->namedPairs->end()){
                                    name = it_Info->second;
                                }else{
                                    QMessageBox::critical(nullptr,
                                                          "Error",
                                                          QString("You must special the pair with label string: %1").arg(str_PatientInfoName.c_str()));
                                    exit(-1);
                                }

                                if(this->thePatientInfoPatten->end() != std::find_if(this->thePatientInfoPatten->begin(),
                                                this->thePatientInfoPatten->end(),
                                                 map_value_finder_PatientInfos(name.c_str()))){
                                    QMessageBox::critical(nullptr,
                                                          "Error",
                                                          QString("The name: %1 repeated").arg(name.c_str()));
                                    exit(-1);
                                }

                                it_Info = it->second->namedPairs->find(str_PatientInfoNecessary);
                                necessary = "false";
                                if(it_Info != it->second->namedPairs->end()){
                                    necessary = it_Info->second;
                                }else{
                                    QMessageBox::critical(nullptr,
                                                          "Error",
                                                          QString("You must special the pair with label string: %1").arg(str_PatientInfoNecessary.c_str()));
                                    exit(-1);
                                }

                                if(necessary != "true" && necessary != "false"){
                                    QMessageBox::critical(nullptr,
                                                          "Error",
                                                          QString("The json file for patient input id: %1 included wrong bool setting: %2")
                                                              .arg(id).arg(necessary.c_str()));
                                    exit(-1);
                                }

                                name.erase(0,name.find_first_not_of(" "));
                                name.erase(name.find_last_not_of(" ")+1);
                                if((signed int)(name.find_first_of(" "))>0){
                                    QMessageBox::critical(nullptr,
                                                          "Error",
                                                          QString("The json file for patient input id: %1 included %2 blank")
                                                              .arg(id).arg(name.c_str()));
                                    exit(-1);
                                }

                                it_Info = it->second->namedPairs->find(str_PatientInfoUnRemoveable);
                                if(it_Info != it->second->namedPairs->end()){
                                    unmremoveable = StringOperation::StringToBool(it_Info->second);
                                }else{
                                    unmremoveable = false;
                                }

                                this->thePatientInfoPatten->insert(std::pair<unsigned int,OnePatientPattern>
                                                                   (id+idShift,OnePatientPattern(labelName.c_str(),name.c_str(),necessary.c_str(),false,unmremoveable)));

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
            std::map<unsigned int,OnePatientPattern>().swap(*this->thePatientInfoPatten);
            delete this->thePatientInfoPatten;
            this->thePatientInfoPatten = NULL;
        }
    }

    return result;
}

void ConfigLoader::writePatientInfoPatternToFile(const std::map<unsigned int,OnePatientPattern> & input){
    std::ifstream ifs;
    std::stringstream ss;
    std::string str_Index;
    JsonExt* ext = new JsonExt();
    JsonBase* base = NULL;
    JsonBase* patientPattern = NULL;
    JsonBase* onePatientInfoLine = NULL;
    unsigned int idShift = FixedPatientInfoPattern::getInstance()->getTheFixedPatientInfoPatten().size();

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

    for(std::map<unsigned int,OnePatientPattern>::const_iterator it = input.begin();
                                                                 it != input.end();
                                                                 it++){
        if(it->first >= idShift){
            ss.str("");
            ss.clear();
            ss<<it->first - idShift;
            str_Index = "";
            ss>>str_Index;
            onePatientInfoLine = new JsonBase();
            onePatientInfoLine->namedPairs = new std::map<std::string,std::string>();
            onePatientInfoLine->namedPairs->insert(std::pair<std::string,std::string>(str_PatientInfoLabel,it->second.labelName.toStdString()));
            onePatientInfoLine->namedPairs->insert(std::pair<std::string,std::string>(str_PatientInfoName,it->second.infoName.toStdString()));
            onePatientInfoLine->namedPairs->insert(std::pair<std::string,std::string>(str_PatientInfoNecessary,it->second.necessary.toStdString()));
            if(true == it->second.unRemoveable){
                onePatientInfoLine->namedPairs->insert(std::pair<std::string,std::string>(str_PatientInfoUnRemoveable,"true"));
            }

            patientPattern->namedObjects->insert(std::pair<std::string, JsonBase*>(str_Index,onePatientInfoLine));
        }
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
    unsigned int index_HintInfo = 0;
    bool result = false;
    std::map<std::string,std::string>::iterator it_pair;
    std::map<std::string,JsonBase*>::iterator it_HintInfo;
    std::ifstream ifs;
    std::map<unsigned int,QString> tempInfos;
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

                        ss.str("");
                        ss.clear();
                        ss<<it->first;
                        ss>>id;

                        if(this->theOperationPatten->find(id) == this->theOperationPatten->end()){
                            if(!it->second){
                                QMessageBox::critical(nullptr,
                                                      "Error",
                                                      QString("The json file for operation pipeline input id: %1 is empty").arg(id));
                                exit(-1);
                            }

                            if(!it->second->namedPairs){
                                QMessageBox::critical(nullptr,
                                                      "Error",
                                                      QString("The pipeline button label %1 not include enough operation setting").arg(id));
                                exit(-1);
                            }

                            it_pair = it->second->namedPairs->find(str_OperationLabel);
                            if(it->second->namedPairs->end() == it_pair){
                                QMessageBox::critical(nullptr,
                                                      "Error",
                                                      QString("The pipeline button label %1 not include any operation label setting").arg(id));
                                exit(-1);
                            }
                            buttonLabel = it_pair->second;

                            it_pair = it->second->namedPairs->find(str_OperationName);
                            if(it->second->namedPairs->end() == it_pair){
                                QMessageBox::critical(nullptr,
                                                      "Error",
                                                      QString("The pipeline button label %1 not include any operation name setting").arg(id));
                                exit(-1);
                            }
                            OperationName = it_pair->second;
                            OperationName.erase(0,OperationName.find_first_not_of(" "));
                            OperationName.erase(OperationName.find_last_not_of(" ")+1);
                            if((signed int)(OperationName.find_first_of(" "))>0){
                                QMessageBox::critical(nullptr,
                                                       "Error",
                                                       QString("The json file for operation pipeline input id: %1 included %2 blank")
                                                              .arg(id).arg(OperationName.c_str()));
                                exit(-1);
                            }

                            it_pair = it->second->namedPairs->find(str_RepeatTimes);

                            if(it->second->namedPairs->end() == it_pair){
                                QMessageBox::critical(nullptr,
                                                      "Error",
                                                      QString("The pipeline button label %1 not include any operation repeat times setting").arg(id));
                                exit(-1);
                            }

                            ss.str("");
                            ss.clear();
                            ss<<it_pair->second;
                            ss>>repeatTimes;
                            if(0 == repeatTimes || (repeatTimes < 0 && -1 != repeatTimes)){
                                QMessageBox::critical(nullptr,
                                                      "Error",
                                                      QString("The json file for operation pipeline input id: %1 included wrong repeat time %2:"
                                                                  "The repeatTime can only be -1(represent nolimitation) or positive integer value")
                                                              .arg(id).arg(repeatTimes));
                                exit(-1);
                            }

                            if(!it->second->namedObjects){
                                QMessageBox::critical(nullptr,
                                                      "Error",
                                                      QString("The pipeline button label %1 not include enough information setting").arg(id));
                                exit(-1);
                            }

                            it_HintInfo = it->second->namedObjects->find(str_HintInfos);

                            std::map<unsigned int,QString>().swap(tempInfos);
                            tempInfos.clear();
                            if(it->second->namedObjects->end() != it_HintInfo){
                                if(it_HintInfo->second && it_HintInfo->second->namedPairs){
                                    for(std::map<std::string,std::string>::iterator it_HintInfo_term = it_HintInfo->second->namedPairs->begin();
                                                                                                       it_HintInfo_term != it_HintInfo->second->namedPairs->end();
                                                                                                       it_HintInfo_term++){
                                        ss.str("");
                                        ss.clear();
                                        ss<<it_HintInfo_term->first;
                                        ss>>index_HintInfo;

                                        if(tempInfos.count(index_HintInfo)){
                                            QMessageBox::critical(nullptr,
                                                                  "Error",
                                                                   QString("The json file for operation pipeline input id: %1 included dumplicated info index %2:")
                                                                           .arg(id).arg(index_HintInfo));
                                            exit(-1);
                                        }

                                        tempInfos.insert(std::pair<unsigned int,QString>(index_HintInfo,it_HintInfo_term->second.c_str()));
                                    }
                                }
                            }

                            this->theOperationPatten->insert(std::pair<unsigned int,OneOperationPattern>
                                                                 (id,OneOperationPattern(QString(buttonLabel.c_str()),
                                                                                         QString(OperationName.c_str()),
                                                                                         repeatTimes,
                                                                                         tempInfos)));
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
    JsonBase* oneHints = NULL;

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
        oneOperation->namedPairs = new std::map<std::string,std::string>();
        oneOperation->namedPairs->insert(std::pair<std::string,std::string>(str_OperationLabel,it->second.buttonLabel.toStdString()));
        oneOperation->namedPairs->insert(std::pair<std::string,std::string>(str_OperationName,it->second.buttonName.toStdString()));
        oneOperation->namedPairs->insert(std::pair<std::string,std::string>(str_RepeatTimes,str_Repeat));

        oneOperation->namedObjects = new std::map<std::string,JsonBase*>();
        oneHints = new JsonBase();
        oneHints->namedPairs = new std::map<std::string,std::string>();

        for(std::map<unsigned int,QString>::const_iterator it_info = it->second.hintInfos.begin();
                                                           it_info != it->second.hintInfos.end();
                                                           it_info++){

            ss.str("");
            ss.clear();
            ss<<it_info->first;
            oneHints->namedPairs->insert(std::pair<std::string,std::string>(ss.str(),it_info->second.toStdString()));
        }

        oneOperation->namedObjects->insert(std::pair<std::string,JsonBase*>(str_HintInfos,oneHints));

        operationPattern->namedObjects->insert(std::pair<std::string, JsonBase*>(str_Index,oneOperation));
    }

    base->namedObjects->insert(std::pair<std::string,JsonBase*>(str_OperationPattern,operationPattern));

    ext->WriteBackToFile(systemCfgPath.toStdString().c_str(),std::ios::ate);

    delete ext;
    ext = NULL;
}

void ConfigLoader::ConstructOnlineDBInfo(){
    std::map<QString,QString>().swap(this->onlineDBInfo);
    this->onlineDBInfo.clear();
    if(!this->readOnlineDatabaseInfoFromFile()){
        this->setDefaultOnlineDatabaseInfo();
    }
}

void ConfigLoader::ConstructPatientInfoPatten(){
    if(this->thePatientInfoPatten){
        std::map<unsigned int,OnePatientPattern>().swap(*this->thePatientInfoPatten);
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

void ConfigLoader::setDefaultOnlineDatabaseInfo(){
    std::map<QString,QString>().swap(this->onlineDBInfo);
    this->onlineDBInfo.clear();

    this->onlineDBInfo.insert(std::pair<QString,QString>(OnlineInfoPattern::getInstance()->getDefalutPattern().at(0),"localhost"));
    this->onlineDBInfo.insert(std::pair<QString,QString>(OnlineInfoPattern::getInstance()->getDefalutPattern().at(1),"3306"));
    this->onlineDBInfo.insert(std::pair<QString,QString>(OnlineInfoPattern::getInstance()->getDefalutPattern().at(2),"tr_patientinfo"));
    this->onlineDBInfo.insert(std::pair<QString,QString>(OnlineInfoPattern::getInstance()->getDefalutPattern().at(3),"root"));
    this->onlineDBInfo.insert(std::pair<QString,QString>(OnlineInfoPattern::getInstance()->getDefalutPattern().at(4),"hfimc"));
}

void ConfigLoader::setDefaultPatientInfoPatten(){
    /*Set unremoveable pattern*/
    unsigned int idShift = FixedPatientInfoPattern::getInstance()->getTheFixedPatientInfoPatten().size();
    if(this->thePatientInfoPatten){
        std::map<unsigned int,OnePatientPattern>().swap(*this->thePatientInfoPatten);
        this->thePatientInfoPatten->clear();
        delete this->thePatientInfoPatten;
        this->thePatientInfoPatten = NULL;
    }
    this->thePatientInfoPatten = new std::map<unsigned int,OnePatientPattern>();

    for(auto it = FixedPatientInfoPattern::getInstance()->getTheFixedPatientInfoPatten().begin();
         it != FixedPatientInfoPattern::getInstance()->getTheFixedPatientInfoPatten().end();
         it++){
        this->thePatientInfoPatten->insert(*it);
    }

    this->thePatientInfoPatten->insert(std::pair<unsigned int,OnePatientPattern>(idShift,OnePatientPattern("PatientName","PatientName","true")));
    this->thePatientInfoPatten->insert(std::pair<unsigned int,OnePatientPattern>(idShift+1,OnePatientPattern("TherapyOrgan","TherapyOrgan","false")));
}

void ConfigLoader::setDefaultOperationPatten(){
    std::map<unsigned int,QString> hintInfos;
    if(this->theOperationPatten){
        std::map<unsigned int,OneOperationPattern>().swap(*this->theOperationPatten);
        this->theOperationPatten->clear();
        delete this->theOperationPatten;
        this->theOperationPatten = NULL;
    }

    this->theOperationPatten = new std::map<unsigned int,OneOperationPattern>();

    std::map<unsigned int,QString>().swap(hintInfos);
    hintInfos.clear();
    this->theOperationPatten->insert(std::pair<unsigned int,OneOperationPattern>(0,OneOperationPattern("Patient come in","PatientComeIn",1,hintInfos)));

    std::map<unsigned int,QString>().swap(hintInfos);
    hintInfos.clear();
    hintInfos.insert(std::pair<unsigned int,QString>(0,"Click again For Imaging/Position end!"));
    this->theOperationPatten->insert(std::pair<unsigned int,OneOperationPattern>(1,OneOperationPattern("Imaging/Position","PatentImaging",2,hintInfos)));

    std::map<unsigned int,QString>().swap(hintInfos);
    hintInfos.clear();
    hintInfos.insert(std::pair<unsigned int,QString>(0,"Click again For next beam!"));
    this->theOperationPatten->insert(std::pair<unsigned int,OneOperationPattern>(2,OneOperationPattern("Therapy","Theraphy",-1,hintInfos)));

    std::map<unsigned int,QString>().swap(hintInfos);
    hintInfos.clear();
    this->theOperationPatten->insert(std::pair<unsigned int,OneOperationPattern>(3,OneOperationPattern("Leaving Room","LeavingRoom",1,hintInfos)));
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
        std::map<unsigned int,OnePatientPattern>().swap(*this->thePatientInfoPatten);
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

    std::map<QString,QString>().swap(this->onlineDBInfo);
    this->onlineDBInfo.clear();
    //this->onlineDBInfo = OnlineInfoPattern::getInstance()->getDefalutPattern();
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
