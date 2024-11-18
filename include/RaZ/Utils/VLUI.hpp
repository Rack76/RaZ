#ifndef VLUI_HPP
#define VLUI_HPP

#include <unordered_map>
#include <cstdint>
#include <cmath>
#include <vector>
#include <type_traits>

namespace Raz
{
    template <typename T>
    class VLUI
    {
    public:
        template <typename ...Types>
        VLUI(Types ...bitPositions)
        {
            if constexpr(std::conjunction<std::is_same<int, Types>...>::value)
            {
                std::vector<int> signature = {bitPositions...};
                for(int bitPosition : signature)
                {
                    setBit(bitPosition);
                }
            }
        }

        void setBit(int position)
        {
            int index = position / 64;
            elements[index] = pow(2, position % 64);
        }

        bool operator<(const VLUI<T> op) const
        {
            auto it1 = elements.end();
            auto it2 = op.elements.end();
            std::advance(it1, -1);
            std::advance(it2, -1);

            for(; it1 != elements.begin() && it2 != op.elements.begin(); )
            {
                if(it1->first == it2->first) 
                {
                    if(it1->second < it2->second)
                        return true;
                    else if(it1->second > it2->second)
                        return false;
                    std::advance(it1, -1);
                    std::advance(it2, -1);
                }
                else if(it1->first < it2->first)
                    return true;
                else
                    return false;
            }

            if(it2 != op.elements.begin())
                return true;

            return false;
        }

    private:
        std::unordered_map<int, std::uint64_t> elements;
    };
}

#endif 