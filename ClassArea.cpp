#include "ClassArea.h"
#include "JavaClass.h"
#include <QFileInfo>

ClassArea::ClassArea(void)
{
}

ClassArea::~ClassArea(void)
{
}

BOOL ClassArea::AddClass(JavaClass* pClass)
{
    if(!pClass) return false;
    QString name=pClass->GetName();

    if (!m_ClassMap.contains(name)) {
        m_ClassMap.insert(name, pClass);
        return true;
    } else {
        cerr<<"Class already exists, calling abort";
        return false;
    }
}

JavaClass* ClassArea::GetClass(QString className)
{   
    if (!m_ClassMap.contains(className)) {
        cerr<<"can't get class "<<className<<endl;
        return (JavaClass*)NULL;
    } else {
        return m_ClassMap.value(className);
    }
}

BOOL ClassArea::LoadClass(QString className, JavaClass *pClass)
{
    if(!pClass) {
        cerr<<"class name does not exist, loadClass aborts"<<endl;
        return false;
    }

    QString fullName = className.append(".class");
    QFileInfo fi(fullName);
    QString absPath = fi.absoluteFilePath();

    if(!(pClass->LoadClassFromFile(absPath))) return false;

	pClass->SetClassHeap(this);

	return AddClass(pClass);
}
