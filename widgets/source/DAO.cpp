#include "../include/DAO.h"

DAO* DAO::thePtr = nullptr;
DAO::GbClear DAO::m_GbClear;

DAO::DAO(){
}

DAO::~DAO(){

}

DAO * DAO::getInstance(){
    if(!this->thePtr){
        this->thePtr = new DAO();
    }
    return this->thePtr;
}

void DAO::clear(){

}

/*Garbge clear*/
DAO::GbClear::GbClear(){

}

DAO::GbClear::~GbClear(){
    if(thePtr){
        delete thePtr;
        thePtr = NULL;
    }
}
