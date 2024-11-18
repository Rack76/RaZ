#ifndef RTTI_H
#define RTTI_H

#include <string>

namespace Raz
{
    class RTTI
    {
    public:
        template <typename T>
        static int id()
        {
            static int typeId = counter++;
            return typeId;
        }

        template <typename Derived, typename Base>
        inline static bool isDerived(Base &derived)
        {
        #ifndef NDEBUG
            return dynamic_cast<Derived*>(&derived);
        #endif
        }

        template <typename T>
        static std::string&& typeName()
        {
           static std::string&& name     = typeid(T).name();
           static std::string   TypeName = name.erase(0, 6);
           return std::move(TypeName);
        }

    private:
        static int counter;
    };
}

#endif