#include "JavaClass.h"
#include "MethodArea.h"
#include "ObjectHeap.h"
#include <QString>
#include <QFile>

JavaClass::JavaClass(void):
    m_pMethodArea(NULL),
	m_pByteCode(NULL),
	m_nByteCodeLength(0),
	m_nObjectFieldsCount(0)
{
}

JavaClass::~JavaClass(void)
{
	// TODO: delete constant_pool, fields, methods and attributes etc.
}


bool JavaClass::LoadClassFromFile(QString filePath)
{ 
    qint64 lenRead, len;

    QFile file(filePath);

    file.open(QIODevice::ReadOnly | QIODevice::Text);

    len=file.size();
    
    m_nByteCodeLength = len;

    char *p = new char[len+2];

    if (p != NULL)
    {
        lenRead=file.read(p, len);
        if (len != lenRead)
        {
            delete p;
            //p = NULL; // For c++ reasons
        }
    }
    else
	{
        file.close();
        return false;
	}
    file.close();

    if (p)
	{
		p[len] = 0;
		SetByteCode(p);
	}

	return (p != NULL);
}

void JavaClass::SetByteCode(void* pByteCode)
{
    //if(m_pByteCode) delete m_pByteCode;
	m_pByteCode=pByteCode;

	if(m_pByteCode) ParseClass();
}

bool JavaClass::ParseClass(void)
{
    if(m_pByteCode==NULL || m_nByteCodeLength < sizeof(JavaClassFileFormat)+20)
        return false;

	char *p=(char *)m_pByteCode;

	magic = getu4(p); p+=4;

	minor_version=getu2(p); p+=2;
    major_version=getu2(p); p+=2;
    constant_pool_count=getu2(p); p+=2;

	if(constant_pool_count>0)
		ParseConstantPool(p);

    access_flags=getu2(p); p+=2;
    this_class=getu2(p); p+=2;
    super_class=getu2(p); p+=2;
    interfaces_count=getu2(p); p+=2;

	if(interfaces_count>0)
		ParseInterfaces(p);

	fields_count=getu2(p); p+=2;

	if(fields_count > 0)
		ParseFields(p);

	methods_count = getu2(p);p+=2;

	if(methods_count > 0)
	{
		ParseMethods(p);
	}

	attributes_count = getu2(p);p+=2;

	if(attributes_count > 0)
		ParseAttributes(p);

	return 0;
}

bool JavaClass::ParseAttributes(char* &p)
{
	attributes = new attribute_info*[attributes_count];

    if(methods == NULL) return false;
	

	for(int i=0;i<attributes_count;i++)
	{
		attributes[i]=(attribute_info*)p;
		u2 name_index=getu2(p);p+=2; //attribute_name_index
		u4 len=getu4(p);p+=4; //len
		p+=len;
	}

    return true;
}

//TODO: Cashe the findings here
bool JavaClass::ParseMethods(char* &p)
{
	methods = new method_info_ex[methods_count];

    if(methods == NULL) return false;
	
	for(int i=0;i<methods_count;i++)
	{
		//methods[i] = new method_info_ex;
		methods[i].pMethodInfoBase=(method_info*)p;
		methods[i].access_flags= getu2(p);	p+=2; //access_flags
		methods[i].name_index = getu2(p); p+=2; //name_index
		methods[i].descriptor_index= getu2(p); p+=2; //descriptor_index
		methods[i].attributes_count=getu2(p); p+=2;
		
        QString strName, strDesc;
		GetStringFromConstPool(methods[i].name_index, strName);
		GetStringFromConstPool(methods[i].descriptor_index, strDesc);

		methods[i].pCode_attr=NULL;
		if(methods[i].attributes_count>0)
		{
			//skip attributes
			for(int a=0;a<methods[i].attributes_count;a++)
			{
				u2 name_index=getu2(p); p+=2;
				
				//printf("Attribute name index = %d\n", name_index);
				u4 len=getu4(p);p+=4;
				p+=len;
			}

			methods[i].pCode_attr = new Code_attribute;
			ParseMethodCodeAttribute(i, methods[i].pCode_attr);
		}		
	}

    return true;
}

bool JavaClass::ParseConstantPool(char* &p)
{	
	constant_pool = new cp_info*[constant_pool_count-1];

    if(constant_pool == NULL) return false;
	

	for(int i=1;i<constant_pool_count;i++)
	{
		constant_pool[i]=(cp_info*)p;
		
		int size = GetConstantPoolSize(p);
		p+= size;

		if(constant_pool[i]->tag == CONSTANT_Long || constant_pool[i]->tag == CONSTANT_Double)
		{
			constant_pool[i+1]=NULL;
			i++;
		}
	}

    return true;
}

bool JavaClass::ParseFields(char* &p)
{
	fields = new field_info_ex[fields_count];

    if(fields == NULL) return false;
	

	for(int i=0;i<fields_count;i++)
	{
		fields[i].pFieldInfoBase = (field_info*)p;

		fields[i].access_flags= getu2(p); p+=2; //access_flags
		fields[i].name_index= getu2(p);p+=2; //
		fields[i].descriptor_index= getu2(p);p+=2; //
		fields[i].attributes_count=getu2(p); p+=2;
		
		if(fields[i].attributes_count>0)
		{
			//skip attributes
			for(int a=0;a<fields[i].attributes_count;a++)
			{
				u2 name_index=getu2(p); p+=2;
				u4 len=getu4(p);p+=4;
				p+=len;
			}
		}
		
	}

    return true;

}
bool JavaClass::ParseInterfaces(char* &p)
{	
	interfaces = new u2[interfaces_count];
	
	for(int i=0;i<interfaces_count;i++)
	{
		interfaces[i] = getu2(p); p+=2;
	}

    return true;
}

int JavaClass::GetConstantPoolSize(char* p)
{
	cp_info* cp=(cp_info *)p;

	switch(cp->tag)
	{
	case CONSTANT_Class:
		return	3;
	case CONSTANT_Fieldref:
		return 5;
	case CONSTANT_Methodref:
		return 5;
	case CONSTANT_InterfaceMethodref:
		return 5;
	case CONSTANT_String:
		return 3;
	case CONSTANT_Integer:
		return 5;
	case CONSTANT_Float:
		return 5;
	case CONSTANT_Long:
		return 9;
	case CONSTANT_Double:
		return 9;
	case CONSTANT_NameAndType:
		return 5;
	case CONSTANT_Utf8:
		return 3+getu2(p+1);
	default:
		break;
	}

	return 0;
}

bool JavaClass::GetConstantPool(u2 nIndex, cp_info& const_pool)
{
    if(nIndex>constant_pool_count-1) return false;

	char *cpool= (char *)constant_pool[nIndex];

	//int nLen=GetConstantPoolSize(cpool);

	const_pool.tag = cpool[0]; cpool++;
	if(CONSTANT_Utf8 == const_pool.tag)
	{
		int len = getu2(cpool);	
		cpool+=2;
		const_pool.info = new u1[len+1];
		memset(const_pool.info, 0, len+1);
		memcpy(const_pool.info , cpool, len);
	}
	if(CONSTANT_Class == const_pool.tag)
	{
		const_pool.info = new u1[2];
		memcpy(const_pool.info , cpool, 2);
	}


	return 0;
}

bool JavaClass::GetStringFromConstPool(int nIndex, QString& strValue)
{

	if(nIndex<1 || nIndex >= constant_pool_count)
	{
        return false;
	}

	u1 *p =(u1 *)constant_pool[nIndex];

    qint8 length=getu2(p+1);
	char *buffer= new char[length+1];
    buffer[length]=0; // (Rui) corresponds to null in ascii

    //memcpy(buffer, &p[3], length); (Rui) just do a substitution here
    memcpy(buffer, p+3, length);
    //memcpy(strValue,buffer,length+1);
    //ASSERT(strlen(strValue)==length+1);
    strValue = QString::fromLocal8Bit(buffer);
	delete buffer;
    return true;
}

QString JavaClass::GetName(void)
{
    QString retVal="";
	if(constant_pool[this_class]->tag != CONSTANT_Class)
		return retVal;
	char *bc=(char*)constant_pool[this_class];
	u2 name_index= getu2(&bc[1]);
	GetStringFromConstPool(name_index, retVal);
	return retVal;
}

QString JavaClass::GetSuperClassName(void)
{
    QString retVal;
	if(super_class<1) return retVal;

	if(constant_pool[super_class]->tag != CONSTANT_Class)
		return retVal;
	char *bc=(char*)constant_pool[super_class];
	u2 name_index= getu2(&bc[1]);
	GetStringFromConstPool(name_index, retVal);
	return retVal;
}

bool JavaClass::ParseMethodCodeAttribute(int nMethodIndex, Code_attribute* pCode_attr)
{
    if(methods==NULL || nMethodIndex > methods_count) return false;
	char *pMi, *bc;
	pMi= bc = (char *)methods[nMethodIndex].pMethodInfoBase;
	bc+=6;
	int nAttributes = getu2(bc); bc+=2;
	
	if(nAttributes>0)
	{
		//skip attributes
		for(int a=0;a<nAttributes;a++)
		{
			u2 name_index=getu2(bc); bc+=2;		
            QString strAttributeName;
			GetStringFromConstPool(name_index, strAttributeName);
			// may be we can compare indexe directly??
            if(!strAttributeName.compare("Code",Qt::CaseInsensitive))
			{
				char *ca=bc;
				pCode_attr->attribute_name_index=name_index;//already scanned;
				pCode_attr->attribute_length=getu4(ca); ca+=4;
				pCode_attr->max_stack=getu2(ca); ca+=2;
				pCode_attr->max_locals=getu2(ca); ca+=2;
				pCode_attr->code_length=getu4(ca); ca+=4;
				if(pCode_attr->code_length>0)
				{
					pCode_attr->code = new u1[pCode_attr->code_length];
					memcpy(pCode_attr->code,ca, pCode_attr->code_length);

					/*
					printf("\nCODE\n");
					for(u4 i=0;i<pCode_attr->code_length;i++)
						printf("%d ", pCode_attr->code[i]);
					printf("\nENDCODE\n");
					*/

				}
				else
				{
					// may be native code ??
					pCode_attr->code=NULL;
				}
				ca+=pCode_attr->code_length;
				pCode_attr->exception_table_length = getu2(ca);ca+=2;

				if(pCode_attr->exception_table_length > 0)
				{
					pCode_attr->exception_table = new Exception_table[pCode_attr->exception_table_length];
					for(int ext= 0; ext<pCode_attr->exception_table_length; ext++)
					{
						pCode_attr->exception_table[ext].start_pc = getu2(ca); ca+=2;
						pCode_attr->exception_table[ext].end_pc = getu2(ca); ca+=2;
						pCode_attr->exception_table[ext].handler_pc = getu2(ca); ca+=2;
						pCode_attr->exception_table[ext].catch_type = getu2(ca); ca+=2;
					}
				}				
			}

			u4 len=getu4(bc);bc+=4;
			bc+=len;
		}
	}		


	return 0;
}

int JavaClass::GetMethodIndex(QString strMethodName, QString strMethodDesc,JavaClass* &pClass)
{
    if(methods == NULL) return false;
	
	JavaClass* pCurClass=this;
	while(pCurClass)
	{
		//_tprintf(_T("Searching class %s\n"), pCurClass->GetName());
		for(int i=0;i<pCurClass->methods_count;i++)
		{
            QString name, desc;

			pCurClass->GetStringFromConstPool(pCurClass->methods[i].name_index, name);
            if(name.compare(strMethodName,Qt::CaseSensitive)) continue;

			pCurClass->GetStringFromConstPool(pCurClass->methods[i].descriptor_index, desc);

            if(!desc.compare(strMethodDesc,Qt::CaseSensitive))
			{
				if(pClass)
					pClass = pCurClass;

				return i;
			}
		}

		if(pClass != NULL)
		{
			pCurClass = pCurClass->GetSuperClass();
		}
		else
		{
			break;
		}
	}

	return -1;
}

int JavaClass::GetFieldIndex(QString strName, QString& strDesc)
{
    if(fields == NULL) return false;
	
	for(int i=0;i<fields_count;i++)
	{
        QString name, desc;
		GetStringFromConstPool(fields[i].name_index, name);
        if(name.compare(strName,Qt::CaseSensitive)) continue;

		GetStringFromConstPool(fields[i].descriptor_index, desc);

		strDesc =desc;
		return i;
	}

	return -1;
}

u4 JavaClass::GetObjectSize(void)
{
	u4 size= fields_count * sizeof(Variable);
    QString superClass=GetSuperClassName();
    JavaClass *pSuperClass = m_pMethodArea->GetClass(superClass);
	u4 superObjSize=0;
	if(pSuperClass)
		superObjSize=pSuperClass->GetObjectSize();

	size+= superObjSize;
	return size;
}

u4 JavaClass::GetObjectFieldCount(void)
{
    u4 count = fields_count;
    QString superClass=GetSuperClassName();
    JavaClass *pSuperClass = m_pMethodArea->GetClass(superClass);
	u4 superObjFieldCount=0;
	if(pSuperClass)
		superObjFieldCount=pSuperClass->GetObjectFieldCount();

	count+= superObjFieldCount;
	return count;
}

JavaClass* JavaClass::GetSuperClass(void)
{
    QString superClass=GetSuperClassName();
    JavaClass *pSuperClass = m_pMethodArea->GetClass(superClass);

	return pSuperClass;
}

bool JavaClass::CreateObject(u2 index, ObjectHeap *pObjectHeap, Object& object)
{
	char *cp=(char*)this->constant_pool[index];
    //ASSERT(cp[0] == CONSTANT_Class);
    //ASSERT(pObjectHeap);
	if(cp[0] != CONSTANT_Class)
        return false;

	u2 name_index=getu2(&cp[1]);
    QString strClassName;
	if(!this->GetStringFromConstPool(name_index, strClassName))
        return false;

    JavaClass *pNewClass=this->m_pMethodArea->GetClass(strClassName);
    if(pNewClass == NULL) return false;
	object=pObjectHeap->CreateObject(pNewClass);
    return true;
}

bool JavaClass::CreateObjectArray(u2 index, u4 count, ObjectHeap *pObjectHeap, Object& object)
{
	char *cp=(char*)this->constant_pool[index];

	if(cp[0] != CONSTANT_Class)
        return false;

	u2 name_index=getu2(&cp[1]);
    QString strClassName;
	if(!this->GetStringFromConstPool(name_index, strClassName))
        return false;
    JavaClass *pNewClass=this->m_pMethodArea->GetClass(strClassName);
    if(pNewClass == NULL) return false;

	return pObjectHeap->CreateObjectArray(pNewClass, count, object);
}

void JavaClass::showClassInfo(void)
{
    QString name= GetName();
    //cout<<"class name is "<<name<<endl;

    QString superName= GetSuperClassName();
    //cout<<"Super class name is "<<superName<<endl;

    //cout<<"obejct size is "<<GetObjectSize();

    for(int i=1;i<constant_pool_count-1;i++)
    {
        QString strRetVal;
        //cout<<"Pool "<<i<<" Type "<<constant_pool[i]->tag;
        if(1!=constant_pool[i]->tag)
            continue;
        GetStringFromConstPool(i,strRetVal);
        //cout<<"String at pool "<<i<<" is "<<strRetVal<<endl;
    }

    for(int i=0;i<methods_count;i++)
    {
        if(methods[i].pCode_attr != NULL)
        {
            printf("Method %d\nCode Length= %d\n", i,methods[i].pCode_attr->code_length);
            printf("Max stack = %d, Max Locals = %d, Exception table length= %d\nCODE\n",methods[i].pCode_attr->max_stack, methods[i].pCode_attr->max_locals, methods[i].pCode_attr->exception_table_length);

            for(u4 j=0;j<methods[i].pCode_attr->code_length;j++)
                printf("%d ", methods[i].pCode_attr->code[j]);
            printf("\nENDCODE\n");
        }
        else if(methods[i].access_flags && ACC_NATIVE)
        {
            printf("Method %d is native\n",i);
        }
    }

    for(int i=0; i< fields_count; i++)
    {
       QString name;
       QString desc;

        GetStringFromConstPool(fields[i].name_index, name);
        GetStringFromConstPool(fields[i].descriptor_index, desc);
        //wprintf(_T("Filed %d: Name: %s Type: %s\n"),i, name, desc);
    }

    for(int i=0; i< interfaces_count; i++)
    {
        u2 intr=interfaces[i];
        QString name;
        cp_info *pi=constant_pool[intr];
        //ASSERT(pi->tag == CONSTANT_Class);
        char *p=(char *)pi;
        int ni=getu2((char *)(&p[1]));
        GetStringFromConstPool(ni, name);
        //cout<<"Interface "<<i<<" name is "<<name<<endl;
    }
}


