#ifndef OBJECT_POOL_H
#define OBJECT_POOL_H

#include "RaZ/Utils/Alloc.hpp"

namespace Raz
{
    class ObjectPool
    {

    };

    template <typename T>
    class ObjectPoolTemplate : public ObjectPool
    {
    public:
        ObjectPoolTemplate()
        {
            data = new void*[blockCount];
            for(int i = 0; i < blockCount; i++)
            {
                data[i] = ALIGNED_ALLOC(objectCountPerBlock * sizeof(T), 64);
            }
            for(int i = 0; i < blockCount; i++)
            {
                for(int j = 0; j < objectCountPerBlock; j++)
                {
                    reinterpret_cast<T**>(data)[i][j] = T();
                }
            }
        }

        ~ObjectPoolTemplate()
        {
            for(int i = 0; i < blockCount; i++)
            {
                ALIGNED_FREE(data[i]);
            }
            delete[] data;
        }

    private:
        static constexpr int objectCountPerBlock = 64;
        static constexpr int blockCount = 5;
        void **data;
    };
}

#endif