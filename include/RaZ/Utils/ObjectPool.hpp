#ifndef OBJECT_POOL_H
#define OBJECT_POOL_H

#include <unordered_map>
#include <cstring>
#include "RaZ/Utils/AllocUtils.hpp"
#include "RaZ/ECS/Component.hpp"
#include "RaZ/Utils/Logger.hpp"
#include "RaZ/Utils/NameAllocator.hpp"
#include "RaZ/Utils/RTTI.hpp"

using BlockHandle = int;

namespace Raz
{
    class ObjectPool
    {
    public:
        virtual Component* getComponent(int blockHandle, Component &component) = 0;
        virtual ~ObjectPool(){}
    };

    template <typename T>
    class ObjectPoolTemplate : public ObjectPool
    {
    public:
        ObjectPoolTemplate()
        {
            data = ALIGNED_ALLOC(poolSize, lineSize);
            if constexpr (sizeof(T) > cacheLineSize)
            {
                T object;
                void* iterator = data;
                for(int lineIndex = 0; lineIndex < blockCount * lineCountPerBlock; lineIndex++)
                {
                    std::memcpy(iterator + lineIndex * lineSize, reinterpret_cast<void*>(&T), sizeof(T));
                }
            }
            else
            {
                T object;
                void* iterator = data;
                for(int lineIndex = 0; lineIndex < blockCount * lineCountPerBlock; lineIndex++)
                {
                    for(int objectIndex = 0 ; objectIndex < packSize; objectIndex++)
                    {
                        std::memcpy(reinterpret_cast<char*>(iterator) + lineIndex * lineSize + objectIndex * sizeof(T), reinterpret_cast<char*>(&object), sizeof(T));
                    }
                }
                if constexpr (useRemainder)
                {
                    for(int objectIndex = 0 ; objectIndex < remainder; objectIndex++)
                    {
                        std::memcpy(data + poolSize + objectIndex * sizeof(T), reinterpret_cast<void*>(&T), sizeof(T));
                    }
                }
            }
        }

        virtual Component* getComponent(int blockHandle, Component &component)
        {
            Logger::conditionalDebug(RTTI::isDerived<T>(component), RTTI::typeName<T>() + " does not inherit from Component");
            int objectId = nameAllocators[blockHandle].getName();
            T* object;
            int unusedBytes = (objectId / packSize) * remainder;
            int blockRelativeObjectOffset = objectId * sizeof(T) + unusedBytes;
            int objectOffset = blockHandle * blockSize + blockRelativeObjectOffset;
            object = reinterpret_cast<T*>(reinterpret_cast<char*>(data) + objectOffset);
            *object = *reinterpret_cast<T*>(&component);
            objectIds[object] = objectId;
            return reinterpret_cast<Component*>(object);
        }

        ~ObjectPoolTemplate()
        {
            ALIGNED_FREE(data);
        }

    private:
        static constexpr int getRemainder()
        {
            constexpr int returnValue = lineSize % packSize;
            if constexpr (returnValue != 0)
                return returnValue;
            else
                return 0;
        }

        static constexpr bool getUseRemainder()
        {
            constexpr int _remainder = lineSize % packSize;
            if constexpr (_remainder != 0)
                return true;
            else
                return false;
        }

        static constexpr int getPoolSize()
        {
            if constexpr (sizeof(T) < cacheLineSize)
            {
                constexpr int returnValue = blockCount * (lineCountPerBlock + 1) * lineSize;
                return returnValue;
            }
            else
            {
                constexpr int returnValue = blockCount * lineCountPerBlock * lineSize;
                return returnValue;
            }
        }

        static constexpr int getBlockSize()
        {
            if constexpr (sizeof(T) < cacheLineSize)
            {
                constexpr int returnValue = (lineCountPerBlock + 1) * lineSize;
                return returnValue;
            }
            else
            {
                constexpr int returnValue = lineCountPerBlock * lineSize;
                return returnValue;
            }
        }

        static constexpr int cacheLineSize = 64;
        static constexpr int lineSize = cacheLineSize * ((sizeof(T) / cacheLineSize) + 1 - ((-(int)sizeof(T) % cacheLineSize) + cacheLineSize) / cacheLineSize);
        static constexpr int objectCountPerBlock = 64;
        static constexpr int blockCount = 5;
        static constexpr int packSize = lineSize / sizeof(T);
        static constexpr int lineCountPerBlock = (objectCountPerBlock / packSize);
        static constexpr int poolSize = getPoolSize();
        static constexpr int blockSize = getBlockSize();
        static constexpr bool useRemainder = getUseRemainder();
        static constexpr int remainder = getRemainder();
        void *data;
        std::unordered_map<BlockHandle, NameAllocator> nameAllocators; 
        std::unordered_map<T*, int> objectIds; 
    };
}

#endif