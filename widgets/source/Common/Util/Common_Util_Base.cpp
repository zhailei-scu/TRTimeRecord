#include "../../../include/Common/Util/Common_Util_Base.h"
#include <algorithm>
#include <sstream>
#include <string>
#include <regex>
#include <QMessageBox>
#include <QString>

using namespace std;

void StringOperation::getAllNumsInString(const char* Str,vector<string> & result,bool hasNegative){
    string temp(Str);
    int startPos = 0;
    int endPos = 0;
    bool finded = false;
    vector<string>().swap(result);
    result.clear();

    for(string::iterator it = temp.begin(); it != temp.end();it++){
        if(*it >= '0' && *it <= '9'){
            finded = true;
        }else if('.' == *it){
            if((it+1) != temp.end() && *(it+1) >= '0' && *(it+1) <= '9'){               // .x
                finded = true;
            }else if(true==finded){                                                     // x.
                finded = true;
            }
        }else if('e' == *it){
            if((it+1) != temp.end() && *(it+1) >= '0' && *(it+1) <= '9'){               // ex
                finded = true;
            }else if(true==finded){                                                     // xe or x.e
                finded = true;
            }
        }else if('-' == *it && hasNegative){
            if((it+1) != temp.end()){
                if(*(it+1) >= '0' && *(it+1) <= '9'){                                   // -x
                    finded = true;
                }else if((it+2) != temp.end() &&
                           '.' == *(it+1) &&
                           *(it+2) >= '0' && *(it+2) <= '9'){                             // -.x
                    finded = true;
                }
            }
        }else if('+' == *it){
            if((it+1) != temp.end()){
                if(*(it+1) >= '0' && *(it+1) <= '9'){                                   // -x
                    finded = true;
                }else if((it+2) != temp.end() &&
                           '.' == *(it+1) &&
                           *(it+2) >= '0' && *(it+2) <= '9'){                             // +.x
                    finded = true;
                }
            }
        }else{
            finded = false;
        }

        if((!finded) && (startPos<endPos)){
            result.push_back(temp.substr(startPos,endPos-startPos));
            startPos = endPos;
        }

        endPos++;

        if(!finded){
            startPos = endPos;
        }

    }

    if(startPos<endPos){
        result.push_back(temp.substr(startPos,endPos-startPos));
    }

}


void StringOperation::getAllNumsInString(const string str,vector<string> & result,bool hasNegative){
    getAllNumsInString(str.c_str(),result,hasNegative);
}

void StringOperation::split_notIncludeFlag(const char* Str,char split,vector<string> & result){
    stringstream ss;
    string temp;
    ss << Str;

    vector<string>().swap(result);
    std::string().swap(temp);

    while(getline(ss,temp,split)){
        if("" != temp && " " != temp){
            result.push_back(temp);
            string().swap(temp);
        }
    }

}

void StringOperation::split_includeFlag(const char* Str,char split,vector<string> & result){
    string ss_Str(Str);
    string temp;

    vector<string>().swap(result);
    string().swap(temp);

    string::iterator it = ss_Str.begin();

    unsigned int posBefore = 0;
    unsigned int pos = 0;
    while(it != ss_Str.end()){

        if(*it == split){

            if(pos>posBefore){
                temp = ss_Str.substr(posBefore,pos-posBefore);
                result.push_back(temp);
            }

            posBefore=pos;
            temp = ss_Str.substr(pos,1);
            result.push_back(temp);
            posBefore = pos + 1;
        }

        pos++;

        it++;

    }

    if(posBefore<ss_Str.size()){
        temp = ss_Str.substr(posBefore,pos-posBefore);
        result.push_back(temp);
    }

}


void StringOperation::EraseHeadAndTail(std::string & line,char theFlag){
    line.erase(0,line.find_first_not_of(theFlag));
    line.erase(line.find_last_not_of(theFlag)+1);
}

void StringOperation::EraseStr(std::string & line,const std::string & str){
    std::regex patten(str,std::regex_constants::icase);
    line = std::regex_replace(line,patten,"$1$3");
}

bool StringOperation::StringToBool(const char* str){
    std::string tempStr = str;
    bool result = false;

    transform(tempStr.begin(),tempStr.end(),tempStr.begin(),::tolower);

    if("true" == tempStr){
        result = true;
    }else if("false" == tempStr){
        result = false;
    }else{
        QMessageBox::critical(nullptr,"Error",QString("Unknown string to bool: %1").arg(str));
        exit(-1);
    }

    return result;
}

bool StringOperation::StringToBool(const string & str){
    return StringToBool(str.c_str());
}

int StringOperation::extractAppendNum(const std::string & line, const std::string & targetStr){
    std::regex patten(targetStr + "\\d{1,}",std::regex_constants::icase); //ignone the upper/low case
    auto word_begin = std::sregex_iterator(line.begin(),line.end(),patten);
    auto word_end = std::sregex_iterator();
    std::string matchedStr;
    std::vector<std::string> splitedNum;
    std::stringstream ss;
    int result = 0;

    if(1 == std::distance(word_begin,word_end)){
        matchedStr = word_begin->str();
        StringOperation::getAllNumsInString(matchedStr,splitedNum,false);
        ss.str("");
        ss.clear();
        ss<<splitedNum[0];
        ss>>result;
    }else{
        QMessageBox::critical(nullptr,"Error",QString("There are  %1 matched expand number after string %2 . In the line %3")
                                                    .arg(std::distance(word_begin,word_end))
                                                    .arg(targetStr.c_str())
                                                    .arg(line.c_str()));
        exit(-1);
    }

    return result;
    /*
    int result = 0;
    int startPos = line.find_first_of(targetStr,0);
    std::vector<std::string> splitedNum;
    std::stringstream ss;

    if(static_cast<int>(line.length()) <= startPos+static_cast<int>(targetStr.length())){
        std::cerr<<"There are none of number append after target str "<<targetStr<<std::endl;
        std::cerr<<"In string: "<<line<<std::endl;
        exit(-1);
    }
    char firstAppendChar = line.at(startPos+targetStr.length());
    if(firstAppendChar < '0' || firstAppendChar > '9'){
        std::cerr<<"There are none of number append after target str "<<targetStr<<std::endl;
        std::cerr<<"In string: "<<line<<std::endl;
        exit(-1);
    }

    StringOperation::getAllNumsInString(line.substr(startPos+targetStr.length()),splitedNum);
    if(splitedNum.size() < 0){
        std::cerr<<"There are none of number append after target str "<<targetStr<<std::endl;
        std::cerr<<"In string: "<<line<<std::endl;
        exit(-1);
    }

    ss.str("");
    ss.clear();
    ss<<splitedNum[0];
    ss>>result;

    return result;
    */
}
