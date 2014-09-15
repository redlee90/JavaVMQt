#ifndef OBJECT_H
#define OBJECT_H

#include "types.h"
#include <inttypes.h>

class Object
{
public:
    qint64 index;
    uchar type;
};

union Variable
{
    u1 charValue;
    u2 shortValue;
    u4 intValue;
    f4 floatValue;
    quintptr ptrValue;
    Object object;
};

#endif // OBJECT_H
