#include "GarbageCollector.h"
#include "NativeMethods.h"

//Garbage collection code is removed for a while

GarbageCollector::GarbageCollector(void)
{
}

GarbageCollector::~GarbageCollector(void)
{
}

// This method stops the execution of ExecutionEngine. 
// Must return as early as possible.
BOOL GarbageCollector::Execute(RuntimeEnvironment* pRuntimeEnvironment)
{
	CSingleLock singleLock(&pRuntimeEnvironment->criticalSection);
	singleLock.Lock();  // Attempt to lock the shared resource

	if (singleLock.IsLocked())  // Resource has been locked
	{
		//...use the shared resource...
		if(!MarkGarbage(pRuntimeEnvironment))
            return false;
		if(!SweepGarbase(pRuntimeEnvironment))
            return false;

		// unlock the resource for others.
		singleLock.Unlock();
	}
	else
	{
        return false;
	}

    return true;
}

//	Mark all objects. Start from stack. Mark stack elements then the elements
//	stack elements point to and the elements those elements pointing and so on.	
//	We use separate storage to mark objects.

BOOL GarbageCollector::MarkGarbage(RuntimeEnvironment* pRuntimeEnvironment)
{
	m_Objects.RemoveAll();

	// Code removed......

    return false;
}

// Collect the unmarked elements.
// Call finalizer of all objects we are collecting.
BOOL GarbageCollector::SweepGarbase(RuntimeEnvironment* pRuntimeEnvironment)
{
	// Code removed......

	m_Objects.RemoveAll();
    return false;
}
