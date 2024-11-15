#ifndef COMPONENT_MANAGER_H
#define COMPONENT_MANAGER_H

#include <memory>
#include <unordered_map>
#include "RaZ/Utils/RTTI.hpp"
#include "Raz/Utils/ObjectPool.hpp"
#include "RaZ/Utils/Logger.hpp"

using Type = int;

namespace Raz
{
    class ComponentManager
    {
    public:
        template <typename T>
        static void registerComponentType()
        {
            pools[RTTI::id<T>()] = std::make_unique<ObjectPoolTemplate<T>>();
        }

    private:
        static std::unordered_map<int, std::unique_ptr<ObjectPool>> pools;
    };
}

#endif