#ifndef OBJECT_HEAP_H
#define OBJECT_HEAP_H

#include "types.h"
#include <stdbool.h>
#include "Object.h"
#include <QMap>

class JavaClass;
class MethodArea;
class Object;

class ObjectHeap
{
public:
	ObjectHeap(void);
public:
	virtual ~ObjectHeap(void);
public:
    Variable* GetObjectPointer(Object object);
	Object CreateObject(JavaClass* pClass);
    Object CreateStringObject(QString pStrValue, MethodArea *pClassHeap);
	Object CreateNewArray(u1 type, i4 count);
    bool CreateObjectArray(JavaClass* pClass, u4 count, Object& object);
private:
    QMap<u4,Variable*> m_ObjectMap;
    u4 m_nNextObjectID;

};

#endif
