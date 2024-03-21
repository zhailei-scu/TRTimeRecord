#ifndef COMMON_UTIL_BASE_H
#define COMMON_UTIL_BASE_H
#include <map>

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

#endif // COMMON_UTIL_BASE_H
