#include <QCoreApplication>
#include "JavaClass.h"
#include "Frame.h"
#include "Object.h"
#include "ClassLoader.h"
#include "MethodArea.h"
#include "ObjectHeap.h"
#include "ExecutionEngine.h"

void Execute(QString);

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);  

    //if(argc<2) return -1;
    Execute("lib/Test.class");

    return a.exec();
}

void Execute(QString className)
{
    JavaClass* pClass1 = new JavaClass();
    JavaClass* pClass2 = new JavaClass();

    MethodArea methodArea;
    ClassLoader classLoader;
    ObjectHeap objectHeap;

    classLoader.loadClass(className, pClass1);
    classLoader.addClassToMethodArea(pClass1,methodArea);

    classLoader.loadClass("java/lang/Object", pClass2);
    classLoader.addClassToMethodArea(pClass2,methodArea);

    Frame *pFrameStack = new Frame[20];
    Frame::pBaseFrame = pFrameStack;

    memset(pFrameStack, 0, sizeof(Frame)*20);

    Frame::pOpStack = new Variable[100];
    memset(Frame::pOpStack, 0, sizeof(Variable)*100);

    ExecutionEngine ex;
    ex.pMethodArea = &methodArea;
    ex.pObjectHeap = &objectHeap;
    int startFrame=0;

    Object object=objectHeap.CreateObject(pClass1);
    JavaClass *pVirtualClass=pClass1;
    int mindex=pClass1->GetMethodIndex("Entry","()I",pVirtualClass);
    pFrameStack[startFrame].pClass = pVirtualClass;

    pFrameStack[startFrame].pMethod = &pVirtualClass->methods[mindex];
    pFrameStack[startFrame].stack = Frame::pOpStack;
    pFrameStack[startFrame].sp = pFrameStack[startFrame].pMethod->pCode_attr->max_locals;
    pFrameStack[startFrame].stack[0].object = object;

    ex.Execute(&pFrameStack[startFrame]);
}

