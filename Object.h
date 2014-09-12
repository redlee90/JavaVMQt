#ifndef OBJECT_H
#define OBJECT_H

#include "types.h"
#include <inttypes.h>

class Object
{
public:
    u4 index;
    u1 type;
};

union Variable
{
    u1 charValue;
    u2 shortValue;
    u4 intValue;
    f4 floatValue;
    int64_t *ptrValue;
    Object object;
};

#endif // OBJECT_H
