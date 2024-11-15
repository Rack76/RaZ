#include "RaZ/ECS/ComponentManager.hpp"

class MyStruct
{
};

int main()
{
    Raz::ComponentManager::registerComponentType<MyStruct>();
}