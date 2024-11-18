#ifndef COMPONENT_MANAGER_H
#define COMPONENT_MANAGER_H

#include <memory>
#include <unordered_map>
#include "RaZ/Utils/RTTI.hpp"
#include "Raz/Utils/ObjectPool.hpp"
#include "RaZ/Utils/Logger.hpp"
#include "RaZ/ECS/Component.hpp"

using BlockHandle = int;
using ArchetypeId = int;
using ComponentTypeId = int;

namespace Raz
{
    class ComponentManager
    {
    public:
        template <typename T>
        static void registerComponentType()
        {
            Logger::conditionalDebug(!std::is_abstract<T>(), "in ComponentManager::registerComponentType cant register abstract class " + RTTI::typeName<T>());
            if constexpr (!std::is_abstract<T>())
                pools[RTTI::id<T>()] = std::make_unique<ObjectPoolTemplate<T>>();
            typeNames[RTTI::id<T>()] = RTTI::typeName<T>(); 
        }

        static int getBlockHandle(int archetypeId, int componentTypeId)
        {
            Logger::conditionalDebug(pools.find(componentTypeId) != pools.end(), "in ComponentManager::getPoolHandle:: " + typeNames[componentTypeId] + " not registered");
            int blockHandle = blockHandles[archetypeId][componentTypeId] = blockHandleCounters[componentTypeId]++;
            return blockHandle;
        }

        template <typename ComponentType>
        static Component* getComponent(BlockHandle block, ComponentType &component)
        {
            Logger::conditionalDebug(pools.find(RTTI::id<ComponentType>()) != pools.end(), "in ComponentManager::getPoolHandle:: " + RTTI::typeName<ComponentType>() + " not registered");
            return pools[RTTI::id<ComponentType>()].get()->getComponent(block, component);
        }

    private:
        static std::unordered_map<ComponentTypeId, std::unique_ptr<ObjectPool>> pools;
        static std::unordered_map<ArchetypeId, std::unordered_map<ComponentTypeId, BlockHandle>> blockHandles;
        static std::unordered_map<ComponentTypeId, int> blockHandleCounters;
        static std::unordered_map<ComponentTypeId, std::string> typeNames;
    };
}

#endif