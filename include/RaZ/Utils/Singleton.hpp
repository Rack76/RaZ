#ifndef SINGLETON_HPP
#define SINGLETON_HPP

#include <memory>

namespace Raz
{
    template <typename T>
    class Singleton
    {
    public:
        static T* getInstance()
        {
            static std::unique_ptr<T> instance = std::make_unique<T>();
            return instance.get();
        }

    protected:
        Singleton();
        Singleton(const Singleton&) = delete;
    };

    template <typename T>
    Singleton<T>::Singleton(){}
}


#endif