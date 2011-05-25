#include <cassert>
#include <cstdlib>
#include <list>
#include <setjmp.h>
#include <stdint.h>

// adhoc for amd64 only
#define PTR_MANGLE(var) __asm__("xorq %%fs:0x30, %0;" \
                                "rolq $17, %0;" : "=r"(var): "0"(var))

#ifndef CL_THREAD_STACK_SIZE
#define CL_THREAD_STACK_SIZE (16 * 1024)
#endif

class CLThread
{
public:
    struct CLThreadContext
    {
        jmp_buf          jmpbuf;
        CLThreadContext *pCallerContext;
        CLThread*        pOwnerThread;
        uint8_t          stack[CL_THREAD_STACK_SIZE];
    };

    CLThread()
    {
        m_context.pCallerContext = 0;
        m_context.pOwnerThread = this;

        makeContext(&(m_context.jmpbuf),
                    staticMainEntry,
                    &(m_context.stack[CL_THREAD_STACK_SIZE - sizeof(void*)]));

        m_threadList.push_back(this);
    }

    ~CLThread()
    {
        m_threadList.remove(this);
    }

    void wait()
    {
        m_threadList.remove(this);
        m_waitingThreadList.push_back(this);
    }

    void run()
    {
        m_threadList.push_back(this);
        m_waitingThreadList.remove(this);
    }

    static void yield()
    {
        switchContext(m_pCurrentContext->pCallerContext); // switch back to the caller context
    }

    static void start()
    {
        for (;;)
        {
            if (m_threadList.size() == 0)
            {
                if (m_waitingThreadList.size() == 0)
                    return;

                assert(0); // no registered thread or deadlock
                return;
            }

            CLThreadContext here; // the jmp_buff to return here
            m_pCurrentContext = &here;

            for (std::list<CLThread*>::iterator i = m_threadList.begin(); i != m_threadList.end();)
            {
                CLThread* pCLThread = *i++;
                // ** WARNING ** increase i before calling pCLThread->call()
                // pCLThread->call() somtimes removes itself from m_threadList
                pCLThread->call();
            }
        }
    }

private:
    void call()
    {
        switchContext(&m_context);
    }

    static void makeContext(jmp_buf* jmpbuf, void (*pEntry)(void), void* pStack)
    {
        uint64_t stackAddress = reinterpret_cast<uint64_t>(pStack);

        setjmp(*jmpbuf);

        PTR_MANGLE(pEntry);
        PTR_MANGLE(stackAddress);

        jmpbuf[0]->__jmpbuf[7] = (long int)pEntry;
        jmpbuf[0]->__jmpbuf[6] = (long int)stackAddress;
    }

    static void switchContext(CLThreadContext* pContext)
    {
        CLThreadContext* pOldContext = m_pCurrentContext;

        pContext->pCallerContext = pOldContext;
        m_pCurrentContext = pContext;

        if (!setjmp(pOldContext->jmpbuf))
            longjmp(pContext->jmpbuf, 1);
    }

    static void staticMainEntry()
    {
        m_pCurrentContext->pOwnerThread->main();

        m_threadList.remove(m_pCurrentContext->pOwnerThread);
        m_waitingThreadList.remove(m_pCurrentContext->pOwnerThread);
        yield();

        abort(); // never to reach here
    }

    virtual void main() = 0;

    CLThreadContext             m_context; // the context structure for this thread

    static CLThreadContext*     m_pCurrentContext; // the current context structure
    static std::list<CLThread*> m_threadList; // the list of registered CLThreads
    static std::list<CLThread*> m_waitingThreadList; // the list of waiting CLThreads
};


template <class T>
class CLChannel
{
public:
    CLChannel() : m_pSrcMessage(0), m_numOfDstChannels(0) {}
    ~CLChannel() {}

    void copyMessage()
    {
        assert(m_pSrcMessage != 0);
        assert(m_pDstMessageList.size() > 0);

        for (typename std::list<T*>::iterator i = m_pDstMessageList.begin(); i != m_pDstMessageList.end(); ++i)
            *(*i) = *m_pSrcMessage;

        m_pSrcMessage = 0;
        m_pDstMessageList.clear();

        for (typename std::list<CLThread*>::iterator i = m_waitingThreadList.begin(); i != m_waitingThreadList.end(); ++i)
            (*i)->run();

        m_waitingThreadList.clear();
    }

    void waitMessage(CLThread* pCLThread)
    {
        pCLThread->wait();
        m_waitingThreadList.push_back(pCLThread);
        CLThread::yield();
    }

    void exchangeMessage(CLThread* pCLThread)
    {
        if (m_numOfDstChannels > 0)
        {
            if (m_pSrcMessage && m_pDstMessageList.size() == m_numOfDstChannels)
                copyMessage();
            else
                waitMessage(pCLThread);
        }
        else
        {
            for (typename std::list<CLThread*>::iterator i = m_waitingThreadList.begin(); i != m_waitingThreadList.end(); ++i)
                (*i)->run();
            m_waitingThreadList.clear();
        }
    }

    void in(T& message, CLThread* pCLThread)
    {
        m_pDstMessageList.push_back(&message);
        exchangeMessage(pCLThread);
    }

    void out(T& message, CLThread* pCLThread)
    {
        m_pSrcMessage = &message;
        exchangeMessage(pCLThread);
    }

    void addRef()
    {
        ++m_numOfDstChannels;
    }

    void removeRef(CLThread* pCLThread)
    {
        --m_numOfDstChannels;

        // if (m_pSrcMessage && m_pDstMessageList.size() > 0 && m_pDstMessageList.size() == m_numOfDstChannels)
        //     copyMessage();
        exchangeMessage(pCLThread);
    }

    int getRef()
    {
        return m_numOfDstChannels;
    }

private:
    T* m_pSrcMessage;
    unsigned int m_numOfDstChannels;
    std::list<T*> m_pDstMessageList;
    std::list<CLThread*> m_waitingThreadList;
};


template <class T>
class CLChannelHolder
{
public:
};


template <class T>
class CLInputChannel : public CLChannelHolder<T>
{
public:
    CLInputChannel(CLChannel<T>* pCLChannel, CLThread* pCLThread) : m_pCLChannel(pCLChannel), m_pCLThread(pCLThread)
    {
        m_pCLChannel->addRef();
    }
    
    void disconnect()
    {
        m_pCLChannel->removeRef(m_pCLThread);
    }

    void get(T& param)
    {
        m_pCLChannel->in(param, m_pCLThread);
    }

    CLChannel<T>* m_pCLChannel;
    CLThread* m_pCLThread;
};


template <class T>
class CLOutputChannel : public CLChannelHolder<T>
{
public:
    CLOutputChannel(CLChannel<T>* pCLChannel, CLThread* pCLThread) : m_pCLChannel(pCLChannel), m_pCLThread(pCLThread) {}

    void set(T& param)
    {
        m_pCLChannel->out(param, m_pCLThread);
    }

    int getRef()
    {
        return m_pCLChannel->getRef();
    }

    CLChannel<T>* m_pCLChannel;
    CLThread* m_pCLThread;
};
