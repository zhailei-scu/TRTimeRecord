#include "../../../include/Global/Communication/Global_Communication_Record.h"
#include "../../../include/Common/Util/Common_Util_Base.h"

Record* Record::m_theInstance = NULL;
Record::GbClear Record::m_GbClear;

Record::Record(){
    this->clear();
}

Record::~Record(){

}

Record* Record::getInstance(){
    /*LazyModel*/
    if(!m_theInstance){
        m_theInstance = new Record();
    }

    return m_theInstance;
}

void Record::upDatePatientInfoRecord(const std::map<unsigned int,OnePatientPattern> & pattern){
    std::map<unsigned int,std::pair<QString,QString>> tempPatientInfoRecord;
    std::map<unsigned int,std::pair<QString,QString>>::iterator find_it;
    for(std::map<unsigned int,OnePatientPattern>::const_iterator it = pattern.begin();
                                                                 it != pattern.end();
                                                                 it++){
        find_it = std::find_if(this->patientInfoRecord.begin(),
                               this->patientInfoRecord.end(),
                               map_value_finder_PairInValue<unsigned int,QString,QString>(it->second.infoName));
        if(this->patientInfoRecord.end() != find_it){
            tempPatientInfoRecord
                .insert(std::pair<unsigned int,std::pair<QString,QString>>
                        (it->first,
                         std::pair<QString,QString>(it->second.infoName,find_it->second.second)));
        }else{
            tempPatientInfoRecord
                .insert(std::pair<unsigned int,std::pair<QString,QString>>
                        (it->first,
                         std::pair<QString,QString>(it->second.infoName,"")));
        }
    }

    std::map<unsigned int,std::pair<QString,QString>>().swap(this->patientInfoRecord);
    this->patientInfoRecord.clear();
    this->patientInfoRecord = tempPatientInfoRecord;
}

void Record::clear(){
    this->lastTableName = "";
    std::map<unsigned int,std::pair<QString,QString>>().swap(this->patientInfoRecord);
    this->patientInfoRecord.clear();

    std::map<unsigned int,QString>().swap(this->buttonTimeRecord);
    this->buttonTimeRecord.clear();
}

Record::GbClear::GbClear(){

}

Record::GbClear::~GbClear(){
    if(m_theInstance){
        delete m_theInstance;
        m_theInstance = NULL;
    }
}
