#include "RaZ/ECS/ComponentManager.hpp"
#include "RaZ/ECS/EntityManager.hpp"

class ALIGN(8) A : public Raz::Component
{
};

class ALIGN(8) B : public Raz::Component
{
};

class ALIGN(8) C : public Raz::Component
{
};

int main()
{
    Raz::ComponentManager::registerComponentType<A>();
    Raz::ComponentManager::registerComponentType<B>();
    Raz::ComponentManager::registerComponentType<C>();
    Raz::EntityManager* entityManager = Raz::EntityManager::getInstance();
    int entity1 = entityManager->addEntity(A{}, B{}, C{});
    int entity2 = entityManager->addEntity(A{}, B{}, C{});
    int entity3 = entityManager->addEntity(A{}, B{}, C{});
}