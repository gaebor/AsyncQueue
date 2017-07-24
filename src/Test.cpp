#include <iostream>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "aq/AsyncQueue.h"
#include "aq/Clock.h"

#include <thread>
#include <chrono>
#include <string>
#include <memory>
#include <limits>

size_t n = 1 << 19;
size_t sleep_constant = 0;
bool force_halt = false; //force halt

int run = 7;

typedef std::shared_ptr<size_t> ManagedMessage;

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
//typedef AsyncQueue<std::shared_ptr<size_t>, true> ManagedQueue;


HighWaterQueue::LimitBehavior behavior = HighWaterQueue::LimitBehavior::None;
size_t limit = std::numeric_limits<size_t>::max();

int main(int argc, char* argv[])
{
	for (++argv;*argv != NULL;++argv)
	{
        const std::string arg_str(*argv);
        if (arg_str == "-h" || arg_str == "--help")
        {
            printf("usage: %s [options]\n", argv[0]);
            printf("\t-n <int>\tlogarithm of number of elements to enqueue, total elements = 2^n, default is 2^%d=%lu\n", (int)log2(n), n);
            printf("\t-s <int>\tsleep parameter, forces the enqueue to be slower, default is %lu\n", sleep_constant);
            printf("\t-f\tperforms force halt, drops elements from the queue, default is %s\n", force_halt ? "true" : "false");
            printf("\t-l <int>\tqueue length limit, see -b, default is %lu\n", limit);
            printf("\t-b <int>\toverloaded queue behavior, default is %d\n\t\t%d is nothing\n\t\t%d is drop when above limit\n\t\t%d is wait until queue reduces below the limit\n", behavior, HighWaterQueue::LimitBehavior::None, HighWaterQueue::LimitBehavior::Drop, HighWaterQueue::LimitBehavior::Wait);
            printf("\t-r <int>\tbitfield containing which tests should run, default is %d\n", run);
            printf("\t\t1: enqueue all elements, then dequeue all in one thread\n");
            printf("\t\t2: enqueue one elements, then dequeue immediately, do this for all elements (one thread)\n");
            printf("\t\t4: starts enqueue thread, starts dequeue thread, waits for enqueue all, then optionally force halt or waits dequeue\n");
            printf("\t\t8: like above, but starts two enqueue thread: first thread enqueues half of the elements, second thread the other half. Also two dequeue threads\n");
            return 0;
        }
		else if (arg_str == "-n")
			n = (1ull << atoi(*++argv));
		else if (arg_str == "-s")
			sleep_constant = atoi(*++argv);
		else if (arg_str == "-f")
			force_halt = true;
		else if (arg_str == "-r")
			run = atoi(*++argv);
		else if (arg_str == "-l")
			limit = atoi(*++argv);
		else if (arg_str == "-b")
			behavior = HighWaterQueue::LimitBehavior(atoi(*++argv));
	}

	HighWaterQueue queue;
	queue.limitBehavior = behavior;
	queue.queueLimit = limit;
	
	aq::Clock timeStart, timePush, timeConsume;

	ManagedMessage output1, output2;

	if (run & 1)
	{
        std::cerr << "[Test of type 1]" << std::endl;
		size_t absTime1, absTime2;
		size_t outputCount = 0;
		timeStart.Tick();

		for ( size_t i = 0; i < n; ++i)
		{
			queue.EnQueue(messageMaker(i));
		}

		timePush.Tick();
		std::cout << n << " element enqueued in " << (absTime1 = (timeStart.Tock() - timePush.Tock()) * 1000) << " ms" << std::endl;

		timeStart.Tick();

		while (queue.GetSize() > 0)
		{
			if (queue.DeQueue(output1))
				++outputCount;
		}
	
		timeConsume.Tick();
		std::cout << outputCount << " element dequeued in " << (absTime2 = (timeStart.Tock() - timeConsume.Tock()) * 1000) << " ms" << std::endl;
		std::cout << "total time: " << absTime1 + absTime2 << " ms\n" << std::endl;
		queue.Reset();
	}
	if (run & 2)
	{
        std::cerr << "[Test of type 2]" << std::endl;
		timeStart.Tick();

		for (size_t i = 0; i < n; ++i)
		{
			queue.EnQueue(messageMaker(i));

			queue.DeQueue(output1);

		}
		timePush.Tick();
		std::cout << n << " element processed in " << (timeStart.Tock() - timePush.Tock()) * 1000 << " ms\n" << std::endl;

		queue.Reset();
	}
	if (run & 4)
	{
        std::cerr << "[Test of type 4]" << std::endl;
		size_t consumed = 0;
		timeStart.Tick();
		std::thread pusher([&]()
		{
			for (size_t i=0; i < n; ++i)
			{
				queue.EnQueue(messageMaker(i));
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
				++consumed;
				//Poco::Thread::sleep(1);//std::cout << "ok"<<std::endl;
			}
		});

		pusher.join();
		timePush.Tick();

		size_t dropped = 0;
		if (force_halt)
			dropped = queue.WakeUp();
		else
			queue.WaitForEmpty();

		queue.WakeUpIfEmpty();
		consumer.join();

		timeConsume.Tick();

		std::cout << "pushing ended in " << (timeStart.Tock() - timePush.Tock()) * 1000 << " ms" << std::endl;
		std::cout << "receiving ended in " << (timeStart.Tock() - timeConsume.Tock()) * 1000 << " ms" << std::endl;
		std::cout << n << " elements has been queued and " << consumed << " has been dequeued" << std::endl;
		std::cout << "high water was " << (double)queue.GetHighWater()*100 / n << "%" << std::endl;
		std::cout << "dropped " << dropped << " elements\n" << std::endl;

		queue.Reset();
	}
	if (run & 8)
	{
        std::cerr << "[Test of type 8]" << std::endl;
		size_t consumed1 = 0, consumed2 = 0;

		timeStart.Tick();
		std::thread pusher1([&]()
		{
			for (size_t i=0; i < n/2; ++i)
			{
				queue.EnQueue(messageMaker(i));
				if ((sleep_constant) != 0 && (i % sleep_constant == 0))
                    std::this_thread::sleep_for(std::chrono::seconds(1));
			}
		});
		std::thread pusher2([&]()
		{
			for (size_t i=n/2; i < n; ++i)
			{
				queue.EnQueue(messageMaker(i));
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
				++consumed1;
					//Poco::Thread::sleep(1);//std::cout << "ok"<<std::endl;
			}
		});

		std::thread consumer2([&]()
		{
			for (; true;)
			{
				if (!queue.DeQueue(output2))
					break;
				++consumed2;
				//Poco::Thread::sleep(1);//std::cout << "ok"<<std::endl;
			}
		});

		pusher1.join();
		pusher2.join();

		timePush.Tick();

		size_t dropped = 0;
		if (force_halt)
			dropped = queue.WakeUp();
		else
			queue.WaitForEmpty();

		timeConsume.Tick();
		
		std::cout << "pushing ended in " << (timeStart.Tock() - timePush.Tock()) * 1000 << " ms" << std::endl;
		std::cout << "receiving ended in " << (timeStart.Tock() - timeConsume.Tock()) * 1000 << " ms" << std::endl;

		queue.WakeUpIfEmpty();

		consumer1.join();
		consumer2.join();

		std::cout << n << " elements has been queued and " << consumed1 << "+" << consumed2 << "=" << consumed1+consumed2 << " elmenents has been dequeued" << std::endl;
		std::cout << "high water was " << (double)queue.GetHighWater()*100 / n << "%" << std::endl;
		std::cout << "dropped " << dropped << " elements\n" << std::endl;

		queue.Reset();
	}
	return 0;
	}
