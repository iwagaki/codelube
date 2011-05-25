#include <stdint.h>
#include <cstdio>
#include <list>
#include <time.h>

#include "CLThread.h"

class CountThread : public CLThread
{
public:
    CountThread(CLChannel<int>* ch) : m_oChannel(ch, this) {};

    void main()
    {
        for (int i = 0; ; ++i)
        {
            if (m_oChannel.getRef() == 0)
            {
                break;
            }

            m_oChannel.set(i);
        }
    }

private:
    CLOutputChannel<int> m_oChannel;
};


class SleepThread : public CLThread
{
public:
    SleepThread(CLChannel<int>* ch, int time) : m_iChannel(ch, this)
    {
        assert(time >= 0);
        m_time = time;
    };

    void main()
    {
        for (;;)
        {
            int message;

            m_iChannel.get(message);
//            printf("%d: get message = %d\n", m_time, message);
            if (message == m_time)
            {
                printf("%d\n", m_time);
                break;
            }
        }

        m_iChannel.disconnect();
    }

private:
    CLInputChannel<int> m_iChannel;
    int m_time;
};


int main()
{
    srand(time(0));

    CLChannel<int> channel;
    std::list<int> target;

    for (int i = 0; i < 100; ++i)
        target.push_back(rand() % 1000);

    CountThread count(&channel);

    for (std::list<int>::iterator i = target.begin(); i != target.end(); ++i)
        new SleepThread(&channel, *i);

    CLThread::start();

    return 0;
}
