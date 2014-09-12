#ifndef CLASS_AREA_H
#define CLASS_AREA_H

#include <stdbool.h>
#include <QString>
#include <QMap>

class JavaClass;

class ClassArea
{
    QMap<QString,JavaClass*> m_ClassMap;
public:
    ClassArea(void);
public:
    virtual ~ClassArea(void);
public:
    bool AddClass(JavaClass* pJavaClass);
    JavaClass* GetClass(QString strClassName);
    bool LoadClass(QString strClassName, JavaClass *pClass);
};

#endif
