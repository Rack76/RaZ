#include "RaZ/ECS/ComponentManager.hpp"

namespace Raz
{

    std::unordered_map<int, std::unique_ptr<ObjectPool>> ComponentManager::pools;
}