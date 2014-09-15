#include "ClassLoader.h"
#include "JavaClass.h"
#include "MethodArea.h"

ClassLoader::ClassLoader(void) {

}

void ClassLoader::loadClass(QString className, JavaClass *pClass) {
    pClass->LoadClassFromFile(className);
}

void ClassLoader::addClassToMethodArea(JavaClass *pClass, MethodArea &methodArea) {
    methodArea.AddClass(pClass);
}

ClassLoader::~ClassLoader() {

}
