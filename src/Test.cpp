#include <iostream>
#include <cmath>
#include <csignal>
#include <cstdio>

#include <thread>
#include <string>
#include <memory>

#include "aq/AsyncQueue.h"
#include "aq/Clock.h"
#include "ArgParser.h"

typedef size_t PodMessage; 
typedef std::shared_ptr<PodMessage> ManagedMessage;

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

aq::AsyncQueue<ManagedMessage, true> managedQueue;
aq::AsyncQueue<PodMessage, true> podQueue;

template<class QueueType>
static void Test(QueueType& queue, int run, size_t n, size_t sleep_constant, bool force_halt)
{
    typedef typename QueueType::Container::value_type MessageType;
    MessageMaker<MessageType> messageMaker;

    aq::Clock<> timer;

    std::thread pusher1, pusher2, consumer1, consumer2;

    MessageType output1, output2;
    
    size_t enqueued1, enqueued2, dequeued1, dequeued2, dropped;

    double absTime, pushTime, consumeTime;

    auto printer = [&] (){
        printf("Enqueue:%20zu+%20zu=%20zu in %8gsec\n", enqueued1, enqueued2, enqueued1 + enqueued2, pushTime);
        printf("Dequeue:%20zu+%20zu=%20zu in %8gsec\n", dequeued1, dequeued2, dequeued1 + dequeued2, consumeTime);
        printf("--------------------------------------------------------------------------------------\n");
        printf("Highwater: %8.3f%%                      Dropped:%20zu in %8gsec\n", (100.0*queue.GetHighWater()) / n, dropped, absTime);
    };

    if (run & 1)
    {
        enqueued1 = 0;
        enqueued2 = 0;
        dequeued1 = 0;
        dequeued2 = 0;
        dropped = 0;
        std::cout << "[Test of type 1]" << std::endl;
        
        timer.Tick();
        for (size_t i = 0; i < n; ++i)
        {
            if (queue.EnQueue(messageMaker(i)))
                ++enqueued1;
        }
        pushTime = timer.Tock();
        while (queue.GetSize() > 0)
        {
            if (queue.DeQueue(output1))
                ++dequeued1;
        }
        absTime = timer.Tock();
        consumeTime = absTime - pushTime;
        printer();
        queue.Reset();
    }
    if (run & 2)
    {
        std::cout << "[Test of type 2]" << std::endl;
        enqueued1 = 0;
        enqueued2 = 0;
        dequeued1 = 0;
        dequeued2 = 0;
        dropped = 0;
        timer.Tick();
        for (size_t i = 0; i < n; ++i)
        {
            if (queue.EnQueue(messageMaker(i)))
            {
                ++enqueued1;
                if (queue.DeQueue(output1))
                    ++dequeued1;
            }
        }
        consumeTime = pushTime = absTime = timer.Tock();
        printer();
        queue.Reset();
    }
    if (run & 4)
    {
        std::cout << "[Test of type 4]" << std::endl;
        enqueued1 = 0;
        enqueued2 = 0;
        dequeued1 = 0;
        dequeued2 = 0;
        dropped = 0;
        timer.Tick();
        pusher1 = std::thread([&]()
        {
            for (size_t i = 0; i < n; ++i)
            {
                if (queue.EnQueue(messageMaker(i)))
                    ++enqueued1;
                if ((sleep_constant) != 0 && (i % sleep_constant == 0))
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        });
        consumer1 = std::thread([&]()
        {
            while (queue.DeQueue(output1))
                ++dequeued1;
        });

        pusher1.join();
        pushTime = timer.Tock();

        if (force_halt)
            dropped = queue.WakeUp();
        else
            queue.WaitForEmpty();

        queue.WakeUpIfEmpty();
        consumer1.join();

        consumeTime = absTime = timer.Tock();

        printer();
        queue.Reset();
    }
    if (run & 8)
    {
        std::cout << "[Test of type 8]" << std::endl;
        enqueued1 = 0;
        enqueued2 = 0;
        dequeued1 = 0;
        dequeued2 = 0;
        dropped = 0;

        timer.Tick();
        pusher1 = std::thread([&]()
        {
            for (size_t i = 0; i < n / 2; ++i)
            {
                if (queue.EnQueue(messageMaker(i)))
                    ++enqueued1;
                if ((sleep_constant) != 0 && (i % sleep_constant == 0))
                    std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        });
        pusher2 = std::thread([&]()
        {
            for (size_t i = n / 2; i < n; ++i)
            {
                if (queue.EnQueue(messageMaker(i)))
                    ++enqueued2;
                if ((sleep_constant) != 0 && (i % sleep_constant == 0))
                    std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        });

        consumer1 = std::thread([&]()
        {
            while (queue.DeQueue(output1))
                ++dequeued1;
        });

        consumer2 = std::thread([&]()
        {
            while (queue.DeQueue(output2))
                ++dequeued2;
        });

        pusher1.join();
        pusher2.join();

        pushTime = timer.Tock();

        if (force_halt)
            dropped = queue.WakeUp();
        else
            queue.WaitForEmpty();

        queue.WakeUpIfEmpty();

        consumer1.join();
        consumer2.join();

        consumeTime = absTime = timer.Tock();

        printer();
        queue.Reset();
    }
}

int main(int argc, const char* argv[])
{
    size_t queueLimit = std::numeric_limits<size_t>::max();
    int limitBehavior = aq::LimitBehavior::None;
    int run = 7;
    bool managed = false;
    size_t n = 19;
    size_t sleep_constant = 0;
    bool force_halt = false; //force halt

    arg::Parser parser("Test tool for AsyncQueue.");

    parser.AddArg(n, { "-n" }, "logarithm of number of elements to enqueue, total elements = 2^n");
    parser.AddArg(sleep_constant, { "-s" }, "sleep parameter, forces the enqueue to be slower");
    parser.AddFlag(force_halt, { "-f" }, "performs force halt, drops elements from the queue when finished");
    parser.AddFlag(managed, { "-m" }, "uses 'shared_ptr<size_t>' messages instead POD message");
    parser.AddArg(queueLimit, { "-l" }, "queue length limit, see -b");
    parser.AddArg(limitBehavior, { "-b" }, "overloaded queue behavior"
        "\n\t\t0 does nothing\n\t\t1 drops in-queue elements when above limit"
        "\n\t\t2 wait until queue reduces below the limit"
        "\n\t\t3 refuse to enqueue if above the limit","", {0,1,2,3});
    parser.AddArg(run, { "-r" }, "bitfield containing which tests should run"
        "\n\t\t1: enqueue all elements, then dequeue all in one thread"
        "\n\t\t2: enqueue one element, then dequeue immediately, do this for all elements (one thread)"
        "\n\t\t4: starts enqueue thread, starts dequeue thread, waits for enqueue all, then optionally force halt or waits dequeue"
        "\n\t\t8: like above, but starts two enqueue thread: first thread enqueues half of the elements, second thread the other half. Also two dequeue threads"
    );
	
    parser.Do(argc, argv);

    managedQueue.limitBehavior = podQueue.limitBehavior = aq::LimitBehavior(limitBehavior);
    managedQueue.queueLimit = podQueue.queueLimit = queueLimit;

    if (managed)
    {
        Test(managedQueue, run, (size_t)1 << n, sleep_constant, force_halt);
    } else
    {
        Test(podQueue, run, (size_t)1 << n, sleep_constant, force_halt);
    }
	return 0;
	}
