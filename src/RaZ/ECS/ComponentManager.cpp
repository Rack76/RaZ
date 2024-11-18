#include "RaZ/ECS/ComponentManager.hpp"

namespace Raz
{
    std::unordered_map<int, std::unique_ptr<ObjectPool>> ComponentManager::pools;
    std::unordered_map<ArchetypeId, std::unordered_map<ComponentTypeId, BlockHandle>> ComponentManager::blockHandles;
    std::unordered_map<ComponentTypeId, int> ComponentManager::blockHandleCounters;
    std::unordered_map<ComponentTypeId, std::string> ComponentManager::typeNames;
}