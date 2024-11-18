#ifndef COMPONENT_HPP
#define COMPONENT_HPP

#include "Raz/Utils/AllocUtils.hpp"

namespace Raz
{
    class ALIGN(8) Component //use ALIGN(8) on subclasses definitions for better performance
    {
    public:
        Component(){}
        int entityId = -1;
        virtual ~Component(){}
    };
}

#endif