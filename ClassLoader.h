#ifndef CLASSLOADER_H
#define CLASSLOADER_H

#include <QString>

class JavaClass;
class MethodArea;

class ClassLoader {
public:
    ClassLoader(void);
    ~ClassLoader(void);
public:
    void loadClass(QString className,JavaClass *pClass);
    void addClassToMethodArea(JavaClass *pClass, MethodArea &methodArea);
};

#endif // CLASSLOADER_H
