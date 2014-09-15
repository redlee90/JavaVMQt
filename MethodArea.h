#ifndef CLASS_AREA_H
#define CLASS_AREA_H

#include <stdbool.h>
#include <QString>
#include <QMap>

class JavaClass;

class MethodArea
{
    QMap<QString,JavaClass*> m_ClassMap;
public:
    MethodArea(void);
public:
    virtual ~MethodArea(void);
public:
    bool AddClass(JavaClass* pJavaClass);
    JavaClass* GetClass(QString strClassName);
    bool LoadClass(QString strClassName, JavaClass *pClass);
};

#endif
