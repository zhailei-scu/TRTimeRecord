#ifndef COMMON_UTIL_BASE_H
#define COMMON_UTIL_BASE_H
#include <map>
#include <vector>
#include <string>

template<class T1,class T2>
class map_value_finder
{
public:
    map_value_finder( const T2 &cmp_value):m_value(cmp_value){}
    bool  operator ()(const std::pair<T1,T2> &pair)
    {
        return pair.second == m_value;
    }
private:
    const T2 &m_value;
};


template<class T1,class T2,class T3>
class map_value_finder_PairInValue
{
public:
    map_value_finder_PairInValue( const T2 &cmp_value):m_value(cmp_value){}
    bool  operator ()(const std::pair<T1,std::pair<T2,T3>> &pair)
    {
        return pair.second.first == m_value;
    }
private:
    const T2 &m_value;
};

class StringOperation{
public:
    static void split_notIncludeFlag(const char* Str,char split,std::vector<std::string> & result);
    static void split_includeFlag(const char* Str,char split,std::vector<std::string> & result);
    static void EraseHeadAndTail(std::string & line,char theFlag);
    static void EraseStr(std::string & line,const std::string & str);
    static bool StringToBool(const char* str);
    static bool StringToBool(const std::string & str);

    static void getAllNumsInString(const std::string str,std::vector<std::string> & result,bool hasNegative);
    static void getAllNumsInString(const char* str,std::vector<std::string> & result,bool hasNegative);

    static int extractAppendNum(const std::string & line, const std::string & targetStr);
};


#endif // COMMON_UTIL_BASE_H
