#include "RaZ/Utils/ObjectPool.hpp"
#include "RaZ/ECS/ComponentManager.hpp"

class MyStruct
{

};

namespace Raz
{
    void f()
    {
        ComponentManager::registerComponentType<MyStruct>();
    }
}