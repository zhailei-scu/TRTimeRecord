#include "../../../include/Global/Communication/Global_Communication_Record.h"

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

void Record::clear(){
    this->lastTableName = "";
    std::map<unsigned int,QString>().swap(this->patientInfoRecord);
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
