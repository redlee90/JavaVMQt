#include "ObjectHeap.h"
#include "JavaClass.h"
#include "ClassArea.h"
#include <iostream>

using namespace std;

ObjectHeap::ObjectHeap(void)
:m_nNextObjectID(1)
{
}

ObjectHeap::~ObjectHeap(void)
{
}

Object ObjectHeap::CreateObject(JavaClass* pClass)
{
	Object object;	
    object.index = 0;
    //object.index = NULL; (Rui) Doesn't need
	object.type=0;

    if(pClass == NULL) {
        cerr<<"class does not exist, create an empty object"<<endl;
        return object;
    }
	u4 osize= pClass->GetObjectFieldCount()+1; //+1 for class pointer
	Variable *obj = new Variable[osize];
	if(!obj) return object;
	memset(obj, 0, sizeof(Variable)*osize);
    object.index=m_nNextObjectID++;
    obj[0].ptrValue = (int64_t *) pClass;

    m_ObjectMap.insert(object.index,obj);

	return object;
}

Object ObjectHeap::CreateStringObject(QString pStrValue, ClassArea *pClassHeap)
{
	Object object;	
    object.index = 0;
	object.type=0;

	if(pClassHeap == NULL) return object;
    JavaClass *pClass=pClassHeap->GetClass("java/lang/String");

	if(pClass == NULL) return object;	

	object = CreateObject(pClass);
	Variable *pVar= this->GetObjectPointer(object);
	if(pVar==NULL) return object;

    pVar[1].ptrValue=(int64_t*)&pStrValue;

	return object;
}

bool ObjectHeap::CreateObjectArray(JavaClass* pClass, u4 count, Object& object)
{
	Variable *pVar= new Variable[count+1];

	if(pVar)
	{
		memset(pVar, 0, sizeof(Variable)*(count+1));
        pVar[0].ptrValue = (int64_t*)pClass;
	}
	else
        return false;

    object.index=m_nNextObjectID++;
	
    m_ObjectMap.insert(object.index,pVar);

    return true;
}

Variable* ObjectHeap::GetObjectPointer(Object object)
{

    if(!m_ObjectMap.contains(object.index)) {
        return NULL;
    }
    return (Variable*) m_ObjectMap.value(object.index);
}

Object ObjectHeap::CreateNewArray(u1 type, i4 count)
{
	Variable *obj= new Variable[count+1];
	Object object;
    object.index=0;
	object.type=type;

	if(obj)
	{
		memset(obj, 0, sizeof(Variable)*(count+1));
        object.index=m_nNextObjectID++;
		obj[0].intValue = type;

        m_ObjectMap.insert(object.index,obj);
	}

	return object;
}
