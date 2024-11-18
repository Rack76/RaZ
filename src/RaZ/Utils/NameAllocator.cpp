#include "RaZ/Utils/NameAllocator.hpp"
#include <limits>
#include <iostream>

int Raz::NameAllocator::getName()
{
    if (restoredNames.size() == 0)
    {
        if (firstUnusedName != maxCount)
        {
            allocatedNames.insert({ firstUnusedName , firstUnusedName });
            return firstUnusedName++;
        }
        else
        {
            std::cerr << "name not allocated : max count exceeded";
            return -1;
        }
    }
    else
    {
        auto it = restoredNames.begin();
        int returnValue = it->first;
        restoredNames.erase(returnValue);
        allocatedNames.insert({ returnValue, returnValue });
        return returnValue;
    }
}

void Raz::NameAllocator::returnName(int name)
{
    if (allocatedNames.find(name) != allocatedNames.end())
    {
        allocatedNames.erase(name);
        restoredNames.insert({ name, name });
    }
    else
        std::cerr << "name not returned : name not allocated";
}

bool Raz::NameAllocator::isReserved(int name) {
    if (allocatedNames.find(name) != allocatedNames.end())
        return true;
    return false;
}

bool Raz::NameAllocator::noNameReserved()
{
    return allocatedNames.size() == 0;
}

void Raz::NameAllocator::setMaxCount(int _maxCount) {
    maxCount = maxCount;
}

int Raz::NameAllocator::lastNameReserved()
{
    if (allocatedNames.size() == 0)
        return -1;
    return std::prev(allocatedNames.end())->first;
}

std::unordered_map<int, int>& Raz::NameAllocator::data()
{
    return allocatedNames;
}
