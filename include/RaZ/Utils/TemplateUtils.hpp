#ifndef TEMPLATE_UTILS_HPP
#define TEMPLATE_UTILS_HPP

namespace Raz
{
    template <int i, typename A, typename B, typename ...Types> 
    constexpr bool isIn() //returns true if A is in the set {B, Types...}
    {
        if constexpr (i == sizeof...(Types) + 1)
            return false;

        else if constexpr (std::is_same<A, B>::value)
            return true;

        else
            return isIn<i+1, A, Types..., B>();
    }

    template <typename T, typename ...Types> 
    constexpr bool duplicates()
    {
        if constexpr (sizeof...(Types) == 1)
        {
            if constexpr (isIn<0, T, Types...>()) 
                return true;
            else
                return false;
        }
        else if constexpr (duplicates<Types...>())
            return true;
        else 
        {
            if constexpr (isIn<0, T, Types...>())
                return true;
            else 
                return false;
        }
    }
}

#endif