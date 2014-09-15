#include "MethodArea.h"
#include "JavaClass.h"
#include <QFileInfo>

MethodArea::MethodArea(void)
{
}

MethodArea::~MethodArea(void)
{
}

bool MethodArea::AddClass(JavaClass* pClass)
{
    if(!pClass) return false;
    QString name=pClass->GetName();

    if (!m_ClassMap.contains(name)) {
        m_ClassMap.insert(name, pClass);
        return true;
    } else {

        return false;
    }
}

JavaClass* MethodArea::GetClass(QString className)
{   
    if (!m_ClassMap.contains(className)) {
        return (JavaClass*)NULL;
    } else {
        return m_ClassMap.value(className);
    }
}

bool MethodArea::LoadClass(QString className, JavaClass *pClass)
{
    if(!pClass) {
        return false;
    }

    QString fullName = className.append(".class");
    QFileInfo fi(fullName);
    QString absPath = fi.absoluteFilePath();

    if(!(pClass->LoadClassFromFile(absPath))) return false;

    pClass->SetMethodArea(this);

	return AddClass(pClass);
}
