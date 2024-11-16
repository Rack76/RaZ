#ifndef ALLOC_H
#define ALLOC_H

#if defined _MSC_VER && !__llvm__ && !__INTEL_COMPILER || __MINGW64__ 
    #include <cstdlib>
    #define ALIGNED_ALLOC(size, alignment) _aligned_malloc(size, alignment)
    #define ALIGNED_FREE(pointer) _aligned_free(pointer)
#else
    #error "unrecognized compiler, define the ALIGNED_ALLOC macro for your compiler inside Alloc.hpp" 
#endif

#endif