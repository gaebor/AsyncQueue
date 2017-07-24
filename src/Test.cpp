#include <iostream>
#include <math.h>
#include <csignal>
#include <stdio.h>

#include "aq/AsyncQueue.h"
#include "aq/Clock.h"

#include <thread>
#include <string>
#include <memory>

size_t n = 1 << 19;
size_t sleep_constant = 0;
bool force_halt = false; //force halt

int run = 7;

typedef std::shared_ptr<size_t> ManagedMessage;
typedef size_t PodMessage;

template<class Type>
struct MessageMaker
{
	Type operator()(const Type& i)
	{
		return i;
	}
};

template<class Type>
struct MessageMaker<std::shared_ptr<Type>>
{
	std::shared_ptr<Type> operator()(const Type& i)
	{
		return std::shared_ptr<Type>(new Type(i));
	}
};

MessageMaker<ManagedMessage> messageMaker;

typedef aq::AsyncQueue<ManagedMessage, true> HighWaterQueue;

HighWaterQueue queue;

//void halt_handle(int param)
//{
//    queue.WakeUp();
//    running = 0;
//    signal(param, SIG_DFL);
//}

int main(int argc, char* argv[])
{


    ++argv; --argc;

	for (;*argv != NULL;++argv, --argc)
	{
        const std::string arg_str(*argv);
        if (arg_str == "-h" || arg_str == "--help")
        {
            printf("usage: %s [options]\n", argv[0]);
            printf("\t-n <int>\tlogarithm of number of elements to enqueue, total elements = 2^n, default is 2^%d=%zu\n", (int)log2(n), n);
            printf("\t-s <int>\tsleep parameter, forces the enqueue to be slower, default is %zu\n", sleep_constant);
            printf("\t-f\tperforms force halt, drops elements from the queue, default is %s\n", force_halt ? "true" : "false");
            printf("\t-l <int>\tqueue length limit, see -b, default is %zu\n", (size_t)queue.queueLimit);
            printf("\t-b <int>\toverloaded queue behavior, default is %d\n"
                "\t\t%d does nothing\n"
                "\t\t%d drop when above limit\n"
                "\t\t%d wait until queue reduces below the limit\n"
                "\t\t%d refuse to enqueue if above the limit\n",
                (HighWaterQueue::LimitBehavior)queue.limitBehavior,
                HighWaterQueue::LimitBehavior::None, HighWaterQueue::LimitBehavior::Drop,
                HighWaterQueue::LimitBehavior::Wait, HighWaterQueue::LimitBehavior::Refuse);
            printf("\t-r <int>\tbitfield containing which tests should run, default is %d\n", run);
            printf("\t\t1: enqueue all elements, then dequeue all in one thread\n");
            printf("\t\t2: enqueue one elements, then dequeue immediately, do this for all elements (one thread)\n");
            printf("\t\t4: starts enqueue thread, starts dequeue thread, waits for enqueue all, then optionally force halt or waits dequeue\n");
            printf("\t\t8: like above, but starts two enqueue thread: first thread enqueues half of the elements, second thread the other half. Also two dequeue threads\n");
            return 0;
        }
        else if (arg_str == "-n" && argc > 0)
        {
            n = (((size_t)1) << atoi(*++argv));
            --argc;
        }
        else if (arg_str == "-s" && argc > 0)
        {
            sleep_constant = atoi(*++argv);
            --argc;
        }
		else if (arg_str == "-f")
			force_halt = true;
        else if (arg_str == "-r" && argc > 0)
        {
            run = atoi(*++argv);
            --argc;
        }
        else if (arg_str == "-l" && argc > 0)
        {
            queue.queueLimit = atoi(*++argv);
            --argc;
        }
        else if (arg_str == "-b" && argc > 0)
        {
            queue.limitBehavior = HighWaterQueue::LimitBehavior(atoi(*++argv));
            --argc;
        }
	}
	
	aq::Clock timeStart, timePush, timeConsume;

	ManagedMessage output1, output2;

	if (run & 1)
	{
        size_t enqueued = 0;
        size_t dequeued = 0;
        std::cerr << "[Test of type 1]" << std::endl;
		double absTime1, absTime2;
		timeStart.Tick();

		for ( size_t i = 0; i < n; ++i)
		{
            if (queue.EnQueue(messageMaker(i)))
                ++enqueued;
		}

		timePush.Tick();
		std::cout << enqueued << " element enqueued in " << (absTime1 = (timeStart.Tock() - timePush.Tock())) << " secs" << std::endl;

		timeStart.Tick();

		while (queue.GetSize() > 0)
		{
			if (queue.DeQueue(output1))
				++dequeued;
		}
	
		timeConsume.Tick();
		std::cout << dequeued << " element dequeued in " << (absTime2 = (timeStart.Tock() - timeConsume.Tock())) << " secs" << std::endl;
		std::cout << "total time: " << absTime1 + absTime2 << " secs\n" << std::endl;
		queue.Reset();
	}
	if (run & 2)
	{
        std::cerr << "[Test of type 2]" << std::endl;
		timeStart.Tick();
        size_t enqueued = 0;
        size_t dequeued = 0;

		for (size_t i = 0; i < n; ++i)
		{
            if (queue.EnQueue(messageMaker(i)))
            {
                ++enqueued;
                if (queue.DeQueue(output1))
                    ++dequeued;
            }
		}
		timePush.Tick();
		std::cout << enqueued << " element enqueued and " << dequeued << " dequeued in " << (timeStart.Tock() - timePush.Tock()) << " secs\n" << std::endl;

		queue.Reset();
	}
	if (run & 4)
	{
        std::cerr << "[Test of type 4]" << std::endl;
        size_t enqueued = 0;
        size_t dequeued = 0;
        size_t dropped = 0;
		timeStart.Tick();
		std::thread pusher([&]()
		{
			for (size_t i=0; i < n; ++i)
			{
				if (queue.EnQueue(messageMaker(i)))
                    ++enqueued;
				if ((sleep_constant) != 0 && (i % sleep_constant == 0))
					std::this_thread::sleep_for(std::chrono::seconds(1));
			}
		});
		std::thread consumer([&]()
		{
			for (; true;)
			{
				if (!queue.DeQueue(output1))
					break;
				++dequeued;
			}
		});

		pusher.join();
		timePush.Tick();

		if (force_halt)
			dropped = queue.WakeUp();
		else
			queue.WaitForEmpty();

		queue.WakeUpIfEmpty();
		consumer.join();

		timeConsume.Tick();

		std::cout << "pushing ended after " << (timeStart.Tock() - timePush.Tock()) << "secs" << std::endl;
		std::cout << "receiving ended after " << (timeStart.Tock() - timeConsume.Tock()) << " secs" << std::endl;
		std::cout << enqueued << " elements has been enqueued and " << dequeued << " has been dequeued" << std::endl;
		std::cout << "high water was " << (double)queue.GetHighWater()*100 / n << "%" << std::endl;
		std::cout << "dropped " << dropped << " elements\n" << std::endl;

		queue.Reset();
	}
	if (run & 8)
	{
        std::cerr << "[Test of type 8]" << std::endl;
        size_t enqueued1 = 0;
        size_t dequeued1 = 0;
        size_t enqueued2 = 0;
        size_t dequeued2 = 0;
        size_t dropped = 0;

		timeStart.Tick();
		std::thread pusher1([&]()
		{
			for (size_t i=0; i < n/2; ++i)
			{
                if (queue.EnQueue(messageMaker(i)))
                    ++enqueued1;
				if ((sleep_constant) != 0 && (i % sleep_constant == 0))
                    std::this_thread::sleep_for(std::chrono::seconds(1));
			}
		});
		std::thread pusher2([&]()
		{
			for (size_t i=n/2; i < n; ++i)
			{
                if (queue.EnQueue(messageMaker(i)))
                    ++enqueued2;
				if ((sleep_constant) != 0 && (i % sleep_constant == 0))
                    std::this_thread::sleep_for(std::chrono::seconds(1));
			}
		});

		std::thread consumer1([&]()
		{
			for (; true;)
			{
				if (!queue.DeQueue(output1))
					break;
				++dequeued1;
			}
		});

		std::thread consumer2([&]()
		{
			for (; true;)
			{
				if (!queue.DeQueue(output2))
					break;
				++dequeued2;
			}
		});

		pusher1.join();
		pusher2.join();

		timePush.Tick();

		if (force_halt)
			dropped = queue.WakeUp();
		else
			queue.WaitForEmpty();

		timeConsume.Tick();
		
		std::cout << "pushing ended after " << (timeStart.Tock() - timePush.Tock()) << " secs" << std::endl;
		std::cout << "receiving ended after " << (timeStart.Tock() - timeConsume.Tock()) << " secs" << std::endl;

		queue.WakeUpIfEmpty();

		consumer1.join();
		consumer2.join();

        std::cout << enqueued1 << "+" << enqueued2 << "=" << enqueued1 + enqueued2 << " elmenents has been enqueued" << std::endl;
        std::cout << dequeued1 << "+" << dequeued2 << "=" << dequeued1 + dequeued2 << " elmenents has been dequeued" << std::endl;
		std::cout << "high water was " << (double)queue.GetHighWater()*100 / n << "%" << std::endl;
		std::cout << "dropped " << dropped << " elements\n" << std::endl;

		queue.Reset();
	}
	return 0;
	}
