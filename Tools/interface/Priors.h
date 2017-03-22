#include <map>
#include <iostream>
#include <type_traits>

#ifndef PRIORS_H_
#define PRIORS_H_

enum class Prior {Gauss, Box, BoxLeft, BoxRight, BoxAuto, Float, Fixed} ;

template<typename T>
std::ostream& operator<<(typename std::enable_if<std::is_enum<T>::value, std::ostream>::type& stream, const T& e)
{
    return stream << static_cast<typename std::underlying_type<T>::type>(e);
}

#endif /* PRIORS_H_ */
