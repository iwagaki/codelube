#include <stdint.h>
#include <cstdio>
#include <list>

#include "CLThread.h"


class PushThread : public CLThread {
public:
    PushThread(CLChannel<int>* ch) : m_oChannel(ch, this) {};

    void main() {
        for (int i = 0; ; ++i) {
            m_oChannel.set(i);
            printf("push i = %d\n", i);
        }
    }

private:
    CLOutputChannel<int> m_oChannel;
};


class PopThread : public CLThread {
public:
    PopThread(CLChannel<int>* ch) : m_iChannel(ch, this) {};

    void main() {
        for (;;) {
            int message;

            m_iChannel.get(message);
            printf("Pop i = %d\n", message);
        }
    }

private:
    CLInputChannel<int> m_iChannel;
};


int main() {
    CLChannel<int> channel;

    PushThread thread1(&channel);
    PopThread thread2(&channel);

    CLThread::start();

    return 0;
}
