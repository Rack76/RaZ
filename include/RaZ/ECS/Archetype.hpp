#ifndef ARCHETYPE_HPP
#define ARCHETYPE_HPP

#include <vector>
#include <tuple>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include "RaZ/Utils/RTTI.hpp"
#include "RaZ/ECS/ComponentManager.hpp"
#include "RaZ/Utils/NameAllocator.hpp"
#include "RaZ/ECS/Component.hpp"

using EntityId = int;

namespace Raz
{
    class Archetype
    {
    public:
        Archetype(){}
        
        Archetype(int archetypeId, std::vector<int> &&signature, std::vector<int> &&componentTypesSizes) : archetypeId(archetypeId)
        {
            getBlockHandles(archetypeId, signature);
            auto pointerToMaxElement = std::max_element(componentTypesSizes.begin(), componentTypesSizes.end());
            if(*pointerToMaxElement <= 8)
                copyMode = true;
        }

        //Archetype(){}

        template <typename ...ComponentTypes>
        int addEntity(int entityId, ComponentTypes &...components)
        {
            int entityLocalId = entityIdAllocator.getName();
            addComponents(entityId, entityLocalId, components...);
            return entityLocalId;
        }

        int archetypeId;
        std::vector<int> signature;

    private:
        inline void getBlockHandles(int archetypeId, std::vector<int> &_signature)
        {
            for(int componentTypeId : signature)
            {
                blockHandles[componentTypeId] = ComponentManager::getBlockHandle(archetypeId, componentTypeId);
            }
        }

        template <typename ComponentType, typename ...ComponentTypes>
        inline void addComponents(int entityId, int entityLocalId, ComponentType &component, ComponentTypes &...components)
        {
            Component* c = ComponentManager::getComponent(blockHandles[RTTI::id<ComponentType>()], component);
            c->entityId = entityId;
            entities[entityLocalId][RTTI::id<ComponentType>()] = c;
            if constexpr (sizeof...(ComponentTypes) != 0)
                addComponents(entityId, entityLocalId, components...);
        }

        std::unordered_map<ComponentTypeId, BlockHandle> blockHandles;
        std::unordered_map<EntityId, std::unordered_map<ComponentTypeId, Component*>> entities;
        NameAllocator entityIdAllocator;
        bool copyMode = false;
    };
}

#endif 