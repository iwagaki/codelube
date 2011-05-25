#include "CLThread.h"

CLThread::CLThreadContext* CLThread::m_pCurrentContext = 0;
std::list<CLThread*>       CLThread::m_threadList;
std::list<CLThread*>       CLThread::m_waitingThreadList;
