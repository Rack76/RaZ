#ifndef ENTITY_MANAGER_H
#define ENTITY_MANAGER_H

#include "RaZ/Utils/RTTI.hpp"
#include "RaZ/Utils/VLUI.hpp"
#include "RaZ/Utils/Singleton.hpp"
#include "RaZ/ECS/Archetype.hpp"
#include "RaZ/Utils/TemplateUtils.hpp"
#include "RaZ/Utils/Logger.hpp"
#include <vector>
#include <map>
#include <unordered_map>
#include <cstdint>

using EntityId = int;
using EntityLocalId = int;
using EntityTypeId = int;

namespace Raz
{
    class EntityManager : public Singleton<EntityManager>
    {
    public:
        template <typename ...ComponentTypes>
        int addEntity(ComponentTypes &&...components)
        {   
        #ifndef NDEBUG
            if constexpr (duplicates<ComponentTypes...>())
            {
                Logger::debug("EntityManager::addEntity called with duplicated template arguments !");
                return;
            }
        #endif 
            int entityId = entityIdAllocator.getName();
            static VLUI<std::uint64_t> entityType = getEntityType<ComponentTypes...>();
            int entityTypeId = entityTypeIds[entityType] = entityTypeIdCounter++;
            archetypes[entityTypeId] = Archetype(entityTypeId, {RTTI::id<ComponentTypes>()...}, {sizeof(ComponentTypes)...});
            int entityLocalId = archetypes[entityTypeId].addEntity(entityId, components...);
            return entityId;
        }

    private:
        template <typename ...ComponentTypes>
        VLUI<std::uint64_t>& getEntityType()
        {
            static VLUI<std::uint64_t> entityType(RTTI::id<ComponentTypes>()...);
            return entityType;
        }

        std::map<VLUI<std::uint64_t>, EntityTypeId> entityTypeIds;
        int entityTypeIdCounter = 0;
        std::unordered_map<EntityTypeId, Archetype> archetypes;
        std::unordered_map<EntityId, std::pair<EntityTypeId, EntityLocalId>> entities;
        NameAllocator entityIdAllocator;
    };
}

#endif