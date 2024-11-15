#ifndef RTTI_H
#define RTTI_H

namespace Raz
{
    class RTTI
    {
    public:
        template <typename T>
        static int id(T& instance)
        {
            return id<T>();
        }

        template <typename T>
        static int id()
        {
            static int typeId = counter++;
            return typeId;
        }

    private:
        static int counter;
    };
}

#endif