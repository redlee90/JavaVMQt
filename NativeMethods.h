#ifndef NATIVE_METHODS_H
#define NATIVE_METHODS_H


#include "Object.h"

class ClassArea;
class ObjectHeap;
class Frame;

class RuntimeEnvironment
{
public:
	Frame *pFrameStack;
    ClassArea *pClassHeap;
	ObjectHeap *pObjectHeap;

	CCriticalSection criticalSection;
};
Variable Add(RuntimeEnvironment* pRuntimeEnvironment);
Variable Print(RuntimeEnvironment* pRuntimeEnvironment);

Variable String_valueOf_F(RuntimeEnvironment* pRuntimeEnvironment);
Variable String_valueOf_J(RuntimeEnvironment* pRuntimeEnvironment);

Variable StringBuilder_append_String(RuntimeEnvironment* pRuntimeEnvironment);
Variable StringBuilder_append_I(RuntimeEnvironment* pRuntimeEnvironment);
Variable StringBuilder_append_C(RuntimeEnvironment* pRuntimeEnvironment);
Variable StringBuilder_append_Z(RuntimeEnvironment* pRuntimeEnvironment);
Variable StringBuilder_append_Object(RuntimeEnvironment* pRuntimeEnvironment);
Variable StringBuilder_append_F(RuntimeEnvironment* pRuntimeEnvironment);
Variable StringBuilder_append_J(RuntimeEnvironment* pRuntimeEnvironment);

Variable StringBuilder_toString_String(RuntimeEnvironment* pRuntimeEnvironment);
typedef Variable (*pNativeMethod)(RuntimeEnvironment* pRuntimeEnvironment);

#endif
