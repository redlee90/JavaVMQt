#include "ExecutionEngine.h"
#include "JavaClass.h"
#include "types.h"
#include "Frame.h"
#include "opcodes.h"
#include "MethodArea.h"
#include "ObjectHeap.h"

Variable* Frame::pOpStack; //static
Frame* Frame::pBaseFrame;

ExecutionEngine::ExecutionEngine(void)
:	pMethodArea(NULL),
	pObjectHeap(NULL)
{	
}

ExecutionEngine::~ExecutionEngine(void)
{
}

u4 ExecutionEngine::Execute(Frame* pFrameStack)
{
    //ASSERT(pFrameStack);
	Frame* pFrame=&pFrameStack[0];
    //ASSERT(pFrame);

	u1 *bc=pFrame->pMethod->pCode_attr->code + pFrame->pc;	
	
	i4 error=0;
	JavaClass *pClass = pFrame->pClass;
    QString strMethod;
	pClass->GetStringFromConstPool(pFrame->pMethod->name_index, strMethod);

	i4 index=0;
    qint64 longVal;
	while(1)
	{		
		switch(bc[pFrame->pc])
		{
		case nop:
			pFrame->pc++;
			break;

		///////////////// Stack Operations ////////////////
		//Instructions that push a constant onto the stack
		case iconst_m1:
		case iconst_0:
		case iconst_1:
		case iconst_2:
		case iconst_3:
		case iconst_4:
		case iconst_5:
			pFrame->sp++;
			pFrame->stack[pFrame->sp].intValue = (u1)bc[pFrame->pc]-iconst_0; 
			pFrame->pc++;
			break;

		case aconst_null:			
			pFrame->sp++;
            pFrame->stack[pFrame->sp].object.index = 0;
			pFrame->pc++;				
			break;

		case lconst_0:// 9 /*(0x9)*/ 
		case lconst_1:// 10 /*(0xa)*/
			pFrame->sp++;
			pFrame->stack[pFrame->sp].intValue=0;
			pFrame->sp++;
			pFrame->stack[pFrame->sp].intValue=(u1)bc[pFrame->pc]-lconst_0;
			pFrame->pc++;				
			break;

		case bipush:// 16 /*(0x10)*/
			pFrame->sp++;
			pFrame->stack[pFrame->sp].charValue=(u1)bc[pFrame->pc+1];
			pFrame->pc+=2;
			break;
		case sipush:// 17 /*(0x11)*/
			pFrame->sp++;
			pFrame->stack[pFrame->sp].shortValue=getu2(&bc[pFrame->pc+1]);
			pFrame->pc+=3;
			break;

		case ldc: //Push item from constant pool 			
			pFrame->stack[++pFrame->sp] =LoadConstant(pFrame->pClass, (u1)bc[pFrame->pc+1]);
			pFrame->pc+=2;
			break;

		case ldc2_w:// 20 /*(0x14)*/
			index=getu2(&bc[pFrame->pc+1]);
			pFrame->sp++;
			pFrame->stack[pFrame->sp].intValue = getu4(&((char *)pClass->constant_pool[index])[1]);
			pFrame->sp++;
			pFrame->stack[pFrame->sp].intValue = getu4(&((char *)pClass->constant_pool[index])[5]);
			pFrame->pc += 3;
			break;

		//Instructions that load a local variable onto the stack
		case aload:// 25 /*(0x19)*/
			pFrame->sp++;
			pFrame->stack[pFrame->sp]=pFrame->stack[(u1)bc[pFrame->pc+1]];
			pFrame->pc+=2;
			break;

		case iload:// 21 /*(0x15)*/
			pFrame->sp++;
			pFrame->stack[pFrame->sp] = pFrame->stack[(u1)bc[pFrame->pc+1]]; 
			pFrame->pc+=2;
			break;
		case iload_0: //26 Load int from local variable 0 
		case iload_1: //27 Load int from local variable 1 
		case iload_2: //28 Load int from local variable 2 
		case iload_3: //29 Load int from local variable 3 
			pFrame->sp++;
			pFrame->stack[pFrame->sp] = pFrame->stack[(u1)bc[pFrame->pc]-iload_0]; 
			pFrame->pc++;			
			break;


		case lload:// 22 /*(0x16)*/

			break;
		case lload_0:// 30 /*(0x1e) */
		case lload_1:// 31 /*(0x1f) */
		case lload_2:// 32 /*(0x20) */
		case lload_3:// 33 /*(0x21) */
			pFrame->sp++;
			pFrame->stack[pFrame->sp]=pFrame->stack[(u1)bc[pFrame->pc]-lload_0];
			pFrame->sp++;
			pFrame->stack[pFrame->sp]=pFrame->stack[(u1)bc[pFrame->pc]-lload_0+1];
			pFrame->pc++;
			break;

		case fload_0: // 34 /*(0x22)*/ 
		case fload_1: // 35 /*(0x23) */
		case fload_2: // 36 /*(0x24) */
		case fload_3: // 37 /*(0x25)*/
			pFrame->sp++;
			pFrame->stack[pFrame->sp] = pFrame->stack[(u1)bc[pFrame->pc]-fload_0]; 
			pFrame->pc++;						
			break;		

		case aload_0:  //42 Load reference from local variable 0
		case aload_1:  //Load reference from local variable 1
		case aload_2:  //Load reference from local variable 2
		case aload_3:  //Load reference from local variable 3
			pFrame->sp++;
			pFrame->stack[pFrame->sp]= pFrame->stack[(u1)bc[pFrame->pc]-aload_0]; 
			DumpObject(pFrame->stack[pFrame->sp].object);
			pFrame->pc++;
			break;

		case iaload:// 46 /*(0x2e)*/Load int from array
			//..., arrayref, index  => ..., value
			pFrame->stack[pFrame->sp-1]=pObjectHeap->GetObjectPointer(pFrame->stack[pFrame->sp-1].object)[pFrame->stack[pFrame->sp].intValue+1];
			pFrame->sp--;
			pFrame->pc++;
			break;
		case aaload://50 
			//..., arrayref, index  ..., value
			pFrame->stack[pFrame->sp-1]=pObjectHeap->GetObjectPointer(pFrame->stack[pFrame->sp-1].object)[pFrame->stack[pFrame->sp].intValue+1];
			pFrame->sp--;
			pFrame->pc++;
			break;
		//Instructions that store a value from the stack into a local variable
		case astore:// 58 (0x3a)
			pFrame->stack[(u1)bc[pFrame->pc+1]]=pFrame->stack[pFrame->sp--];
			pFrame->pc+=2;
			break;

		case istore:// 54 /*(0x36)*/
			pFrame->stack[(u1)bc[pFrame->pc+1]]=pFrame->stack[pFrame->sp--];			
			pFrame->pc+=2;
			break;
		case istore_0: // 59 /*(0x3b)*/ 
		case istore_1: // 60 /*(0x3c) */
		case istore_2: // 61 /*(0x3d) */
		case istore_3: // 62 /*(0x3e)*/			
			pFrame->stack[(u1)bc[pFrame->pc]-istore_0]=pFrame->stack[pFrame->sp--];			
			pFrame->pc++;
			break;

		case lstore_0: // 63 /*(0x3f) */
		case lstore_1: // 64 /*(0x40) */
		case lstore_2: // 65 /*(0x41) */
		case lstore_3: // 66 /*(0x42) */
			pFrame->stack[(u1)bc[pFrame->pc]-lstore_0+1]=pFrame->stack[pFrame->sp--];
			pFrame->stack[(u1)bc[pFrame->pc]-lstore_0]=pFrame->stack[pFrame->sp--];
			pFrame->pc++;
			break;

		case fstore_0:
		case fstore_1:
		case fstore_2:
		case fstore_3:
			pFrame->stack[(u1)bc[pFrame->pc]-fstore_0]=pFrame->stack[pFrame->sp--];			
			pFrame->pc++;
			break;

		case astore_0:// 75 /*(0x4b) Store reference into local variable 0*/
		case astore_1:// 76 /*(0x4c) */
		case astore_2:// 77 /*(0x4d) */
		case astore_3:// 78 /*(0x4e)*/
			pFrame->stack[(u1)bc[pFrame->pc]-astore_0]=pFrame->stack[pFrame->sp--];			
			pFrame->pc++;
			break;

		case iastore:// 79 /*(0x4f)*/
		case aastore: // 83 - both seems same (TODO: Check it)
			pObjectHeap->GetObjectPointer(pFrame->stack[pFrame->sp-2].object)[pFrame->stack[pFrame->sp-1].intValue+1]=pFrame->stack[pFrame->sp];
			pFrame->sp-=3;
			pFrame->pc++;
			break;

		//Generic (typeless) stack operations

		case dup:// 89 /*(0x59)*/
			pFrame->stack[pFrame->sp+1]=pFrame->stack[pFrame->sp];
			pFrame->sp++;
			pFrame->pc++;
			break;
		case dup_x1:// 90 /*(0x5a)*/
			pFrame->stack[pFrame->sp+1]=pFrame->stack[pFrame->sp];
			pFrame->stack[pFrame->sp]=pFrame->stack[pFrame->sp-1];
			pFrame->stack[pFrame->sp-1]=pFrame->stack[pFrame->sp+1];
			pFrame->sp++;
			pFrame->pc++;
			break;
		case dup_x2:// 91 /*(0x5b)*/
			error=1;
			break;

		//Type Conversion

		//Integer Arithmetic 

		case iadd: //96
			pFrame->stack[pFrame->sp-1].intValue=pFrame->stack[pFrame->sp-1].intValue + pFrame->stack[pFrame->sp].intValue;	
			pFrame->sp--;
			pFrame->pc++;
			break;
		case ladd:// 97 /*(0x61)*/
            longVal = (qint64)(((qint64)pFrame->stack[pFrame->sp-3].intValue<<32) | (qint64)pFrame->stack[pFrame->sp-2].intValue)+(qint64)(((qint64)pFrame->stack[pFrame->sp-1].intValue<<32) | (qint64)pFrame->stack[pFrame->sp].intValue);
			pFrame->stack[pFrame->sp-3].intValue=HIINT64(longVal);
			pFrame->stack[pFrame->sp-2].intValue=LOINT64(longVal);
			pFrame->sp -= 2;
			pFrame->pc++;
			break;
		case isub: //100
			pFrame->stack[pFrame->sp-1].intValue=pFrame->stack[pFrame->sp-1].intValue - pFrame->stack[pFrame->sp].intValue;	
			pFrame->sp--;
			pFrame->pc++;			
			break;
		case imul://104
			pFrame->stack[pFrame->sp-1].intValue=pFrame->stack[pFrame->sp-1].intValue * pFrame->stack[pFrame->sp].intValue;	
			pFrame->sp--;
			pFrame->pc++;			
			break;

		case iinc:// 132 /*(0x84)*/ Increment local variable by constant
			pFrame->stack[(u1)bc[pFrame->pc+1]].intValue += (char)bc[pFrame->pc+2];
			pFrame->pc+=3;
			break;
		////////////////////// Logic ///////////////////
		
		//Shift operations

		//Bitwise boolean operations

		//Floating Point Arithmetic 



		/////////////// Objects and Arrays  ////////////

		//Instructions that deal with objects

		case _new:// 187 (0xbb)
			ExecuteNew(pFrame);
			pFrame->pc+=3;
			break;
		case putfield: //181 (0xb5): Set field in object
			PutField(pFrame);
			pFrame->sp-=2;
			pFrame->pc+=3;
			break;

		case getfield: //180 (0xb4) Fetch field from object
			GetField(pFrame);
			pFrame->pc+=3;
			break;

		//Instructions that deal with arrays

		case newarray:// 188 /*(0xbc)*/
			ExecuteNewArray(pFrame);
			pFrame->pc+=2;
			break;

		case anewarray: //189
			ExecuteANewArray(pFrame);
			pFrame->pc+=3;
			break;
		////////////// Control Flow /////////////////////

		//Conditional branch instructions

		case if_icmpeq: // 159 /*(0x9f) */
			if(pFrame->stack[pFrame->sp -1].intValue == pFrame->stack[pFrame->sp].intValue)
			{
				pFrame->pc+= geti2(&bc[pFrame->pc+1]);
			}
			else
			{
				pFrame->pc+=3;
			}
			pFrame->sp-=2;
			break;
		case if_icmpne: //160 /*(0xa0) */
			if(pFrame->stack[pFrame->sp -1].intValue != pFrame->stack[pFrame->sp].intValue)
			{
				pFrame->pc+= geti2(&bc[pFrame->pc+1]);
			}
			else
			{
				pFrame->pc+=3;
			}
			pFrame->sp-=2;
			break;
		case if_icmplt: // 161 /*(0xa1) */
			if(pFrame->stack[pFrame->sp -1].intValue < pFrame->stack[pFrame->sp].intValue)
			{
				pFrame->pc+= geti2(&bc[pFrame->pc+1]);
			}
			else
			{
				pFrame->pc+=3;
			}
			pFrame->sp-=2;
			break;
		case if_icmpge: // 162 /*(0xa2) */
			if(pFrame->stack[pFrame->sp -1].intValue >= pFrame->stack[pFrame->sp].intValue)
			{
				pFrame->pc+= geti2(&bc[pFrame->pc+1]);
			}
			else
			{
				pFrame->pc+=3;
			}
			pFrame->sp-=2;
			break;
		case if_icmpgt: // 163 /*(0xa3) */
			if(pFrame->stack[pFrame->sp -1].intValue > pFrame->stack[pFrame->sp].intValue)
			{
				pFrame->pc+= geti2(&bc[pFrame->pc+1]);
			}
			else
			{
				pFrame->pc+=3;
			}
			pFrame->sp-=2;
			break;
		case if_icmple: // 164 /*(0xa4)*/
			if(pFrame->stack[pFrame->sp -1].intValue <= pFrame->stack[pFrame->sp].intValue)
			{
				pFrame->pc+= geti2(&bc[pFrame->pc+1]);
			}
			else
			{
				pFrame->pc+=3;
			}
			pFrame->sp-=2;
			break;

		case ifeq:// 153 /*(0x99) */
			if(pFrame->stack[pFrame->sp].intValue == 0)
			{
				pFrame->pc+= geti2(&bc[pFrame->pc+1]);
			}
			else
			{
				pFrame->pc+=3;
			}
			pFrame->sp--;
			break;
		case ifne:// 154 /*(0x9a) */
			if(pFrame->stack[pFrame->sp].intValue != 0)
			{
				pFrame->pc+= geti2(&bc[pFrame->pc+1]);
			}
			else
			{
				pFrame->pc+=3;
			}
			pFrame->sp--;
			break;
		case iflt:// 155 /*(0x9b) */
			if(pFrame->stack[pFrame->sp].intValue < 0)
			{
				pFrame->pc+= geti2(&bc[pFrame->pc+1]);
			}
			else
			{
				pFrame->pc+=3;
			}
			pFrame->sp--;
			break;
		case ifge:// 156 /*(0x9c) */
			if(pFrame->stack[pFrame->sp].intValue >= 0)
			{
				pFrame->pc+= geti2(&bc[pFrame->pc+1]);
			}
			else
			{
				pFrame->pc+=3;
			}
			pFrame->sp--;
			break;
		case ifgt:// 157 /*(0x9d) */
			if(pFrame->stack[pFrame->sp].intValue > 0)
			{
				pFrame->pc+= geti2(&bc[pFrame->pc+1]);
			}
			else
			{
				pFrame->pc+=3;
			}
			pFrame->sp--;
			break;
		case ifle:// 158 /*(0x9e)*/
			if(pFrame->stack[pFrame->sp].intValue <= 0)
			{
				pFrame->pc+= geti2(&bc[pFrame->pc+1]);
			}
			else
			{
				pFrame->pc+=3;
			}
			pFrame->sp--;
			break;
		//Comparison instructions

		//Unconditional branch instructions
		case  _goto: // 167 /*(0xa7)*/
				pFrame->pc += geti2(&bc[pFrame->pc+1]);
			break;
		//Table jumping instructions

		////////////// Exceptions ///////////////////////
		case athrow:
			error =1;
			break;

		//////////////////////// Method Invocation and Return ////////

		//Method invocation instructions
		case invokespecial:
			ExecuteInvokeSpecial(pFrame);			
			pFrame->pc+=3;
			break;

		case invokevirtual: //182
			ExecuteInvokeVirtual(pFrame, invokevirtual);
			pFrame->pc+=3;
			break;

		case invokestatic:// 184 
			ExecuteInvokeVirtual(pFrame, invokestatic);
			pFrame->pc+=3;
			break;
		//Method return instructions
		case ireturn: //172 (0xac)			

			pFrame->stack[0].intValue=pFrame->stack[pFrame->sp].intValue;			
			return ireturn;
			break;

		case _return: //177 (0xb1): Return (void) from method			

			return 0;//METHOD_RETURN;
			break;
		//////////////// Thread Synchronization ////////////////////

		case monitorenter:// Enter and acquire object monitor 
		case monitorexit:// Release and exit object monitor
			error = 1;
			break;

		default:
			error=1;
			break;
		}

		if(error) break;
	}
	
    //ASSERT(!error);

	return 0;
}

void ExecutionEngine::ExecuteInvokeSpecial(Frame* pFrameStack)
{
	ExecuteInvokeVirtual(pFrameStack, invokespecial);
}

Variable ExecutionEngine::LoadConstant(JavaClass *pClass, u1 nIndex)
{
	Variable v;
	v.ptrValue = 0;
    QString pStrVal=NULL, strTemp;

	char* cp=(char *)pClass->constant_pool[nIndex];
	u2 i;
	Object object;
	switch(cp[0])
	{
	case CONSTANT_Integer:
		v.intValue = getu4(&cp[1]);
		break;

	case CONSTANT_Float:
		v.floatValue = getf4((char *)&cp[1]);
		break;

	case CONSTANT_String:
		i=getu2((char *)&cp[1]);	
		pClass->GetStringFromConstPool(i, strTemp);	
        pStrVal.append(strTemp);
        object = this->pObjectHeap->CreateStringObject(pStrVal, pMethodArea);
        v.ptrValue=object.index;
		break;

	case CONSTANT_Double:
		break;

	case CONSTANT_Long:
		break;		
	}
	return v;
}
void ExecutionEngine::PutField(Frame* pFrameStack)
{
	u2 nIndex = getu2(&pFrameStack[0].pMethod->pCode_attr->code[pFrameStack[0].pc+1]);
	Variable obj=pFrameStack[0].stack[pFrameStack[0].sp-1];
	Variable value=pFrameStack[0].stack[pFrameStack[0].sp];
	Variable *pVarList=this->pObjectHeap->GetObjectPointer(obj.object);
	JavaClass *pClass = (JavaClass *)pVarList[0].ptrValue;
    //ASSERT(pClass && pClass->magic == 0xCAFEBABE);
	//ShowClassInfo(pClass);
	pVarList[nIndex+1]=value;
}

void ExecutionEngine::GetField(Frame* pFrame)
{
	u2 nIndex = getu2(&pFrame->pMethod->pCode_attr->code[pFrame->pc+1]);
	Variable obj=pFrame->stack[pFrame->sp];
	
	Variable *pVarList=this->pObjectHeap->GetObjectPointer(obj.object);
	JavaClass *pClass = (JavaClass *)pVarList[0].ptrValue;
    //ASSERT(pClass && pClass->magic == 0xCAFEBABE);
	//ShowClassInfo(pClass);
	pFrame->stack[pFrame->sp]=pVarList[nIndex+1];
}

void ExecutionEngine::ExecuteInvokeVirtual(Frame* pFrameStack, u2 type)
{
	u2 mi=getu2(&pFrameStack[0].pMethod->pCode_attr->code[pFrameStack[0].pc+1]);
	Variable objectRef = pFrameStack[0].stack[pFrameStack[0].sp]; 
	char *pConstPool = (char *)pFrameStack[0].pClass->constant_pool[mi];

    //ASSERT(pConstPool[0] == CONSTANT_Methodref);
		
	u2 classIndex = getu2(&pConstPool[1]);
	u2 nameAndTypeIndex = getu2(&pConstPool[3]);

	//get class at pool index 
	pConstPool = (char *)pFrameStack[0].pClass->constant_pool[classIndex];

    //ASSERT(pConstPool[0] == CONSTANT_Class);

	u2 ni=getu2(&pConstPool[1]);

    QString strClassName;
	pFrameStack[0].pClass->GetStringFromConstPool(ni, strClassName);


    JavaClass *pClass=pMethodArea->GetClass(strClassName);

	//ShowClassInfo(pClass);

	pConstPool = (char *)pFrameStack[0].pClass->constant_pool[nameAndTypeIndex];


    //ASSERT(pConstPool[0] == CONSTANT_NameAndType);


	method_info_ex method;

	
	method.name_index = getu2(&pConstPool[1]);
	method.descriptor_index = getu2(&pConstPool[3]);

	method.access_flags = 0; // todo set 

    QString strName, strDesc;
	pFrameStack[0].pClass->GetStringFromConstPool(method.name_index, strName);
	pFrameStack[0].pClass->GetStringFromConstPool(method.descriptor_index, strDesc);

	
	//printf("SuperClass - %s",(method.access_flags& ACC_SUPER)?"Yes":"No");
	JavaClass *pVirtualClass=pClass;
	int nIndex=pClass->GetMethodIndex(strName, strDesc, pVirtualClass);

	memset(&pFrameStack[1],0,sizeof(pFrameStack[1]));
	pFrameStack[1].pMethod = &pClass->methods[nIndex];
		
	method.access_flags = getu2((char *)pFrameStack[1].pMethod);
	if( ACC_SUPER & method.access_flags)
	{
		pFrameStack[1].pClass = pVirtualClass->GetSuperClass();
		//ShowClassInfo(pFrameStack[1].pClass);
	}
	else
	{
		pFrameStack[1].pClass=pVirtualClass;
	}

	//pFrameStack[1].pOpStack[++pFrameStack[1].sp]=pFrameStack[0].pOpStack[pFrameStack[0].sp--];
	int params=GetMethodParametersStackCount(strDesc)+1;
	
	//static
	if(type==invokestatic) params--;

	int nDiscardStack =params;
	if(pFrameStack[1].pMethod->access_flags & ACC_NATIVE)
	{
	}
	else 
	{
		nDiscardStack+=pFrameStack[1].pMethod->pCode_attr->max_locals; 
	}
	
	pFrameStack[1].stack = &Frame::pOpStack[pFrameStack->stack-Frame::pOpStack+pFrameStack[0].sp-params+1];
	pFrameStack[1].sp=nDiscardStack-1;

	this->Execute(&pFrameStack[1]);

	//if returns then get on stack	
    if(strDesc.contains(")V") < 0)
	{
		nDiscardStack--;		
	}

	pFrameStack[0].sp-=nDiscardStack;
}

u2 ExecutionEngine::GetMethodParametersCount(QString strMethodDesc)
{
	u2 count=0;
	
    int i, len=strMethodDesc.size();

	//todo: long/double takes 2 stack position
	for(i=1;i<len;i++)
	{
        if(strMethodDesc.at(i) =='L')
		{
            while(strMethodDesc.at(i) !=';') i++;
		}
        if(strMethodDesc.at(i) ==')') break;
		count++;
	}

	return count;
}

u2 ExecutionEngine::GetMethodParametersStackCount(QString strMethodDesc)
{
	u2 count=0;
	
    int i, len=strMethodDesc.size();

	//todo: long/double takes 2 stack position
	for(i=1;i<len;i++)
	{
        if(strMethodDesc.at(i) =='L')
		{
            while(strMethodDesc.at(i) !=';') i++;
		}
        if(strMethodDesc.at(i) ==')') break;
        if(strMethodDesc.at(i) =='J' || strMethodDesc.at(i) =='D')
			count++;
		count++;
	}

	return count;
}

int ExecutionEngine::ExecuteNew(Frame* pFrame)
{
	pFrame->sp++;
	u1 *bc=pFrame->pMethod->pCode_attr->code;
	u2 index=getu2(&bc[pFrame->pc+1]);

	if(!pFrame->pClass->CreateObject(index, this->pObjectHeap, pFrame->stack[pFrame->sp].object))
		return -1;
	

	return 0;
}

int ExecutionEngine::ExecuteNewArray(Frame* pFrame)
{
	u1 type=pFrame->pMethod->pCode_attr->code[pFrame->pc+1];
	pFrame->stack[pFrame->sp+1].object = pObjectHeap->CreateNewArray(type, pFrame->stack[pFrame->sp].intValue);
	pFrame->sp++;
	return 0;
}
int ExecutionEngine::ExecuteANewArray(Frame* pFrame)
{
	u2 index=getu2(&pFrame->pMethod->pCode_attr->code[pFrame->pc+1]);
	u4 count=pFrame->stack[pFrame->sp].intValue;
	pFrame->sp++;

	//be carefull abuot stack pointer - its incremented once
	if(!pFrame->pClass->CreateObjectArray(index, count,pObjectHeap, pFrame->stack[pFrame->sp].object))
	{
		return -1;
	}
	
	return 0;
}

Variable* ExecutionEngine::CreateNewArray(u1 type, i4 count)
{
	Variable *pVar= new Variable[count+1];

	if(pVar)
	{
		memset(pVar, 0, sizeof(Variable)*(count+1));
		pVar[0].intValue = type;
	}

	return pVar;
}

void ExecutionEngine::DumpObject(Object object)
{
	return;
#ifdef DEBUG
	JavaClass *cls=(JavaClass *)this->pObjectHeap->GetObjectPointer(object)[0].ptrValue;
	if((LONG_PTR)cls < 15)
	{
		return;
	}

	DbgPrint(_T("Load Class %s\n"),cls->GetName());
#endif
}
