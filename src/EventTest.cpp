#include <iostream>
#include <cstdio>

#include <thread>
#include <string>
#include <vector>

#include "aq/Event.h"
#include "ArgParser.h"

size_t worker(size_t a, size_t b)
{
    size_t result = 0;
    for (size_t i = a; i < b; ++i)
        result |= i;
    return result;
}

struct WorkerArgs
{
    size_t a, b;
    size_t result;
    std::thread _thread;
    aq::Event<> compute, done;
};

void Test(int thread_option, size_t num_threads, size_t n, size_t chunks)
{
    std::vector<WorkerArgs> args(num_threads);
    switch (thread_option)
    {
    case 1:
    {
        size_t a = 0;
        while (a < n)
        {
            for (size_t i = 0; i < num_threads; ++i, a += chunks)
            {
                args[i].a = a;
                args[i].b = a + chunks;
                args[i]._thread = std::thread([](WorkerArgs* arg)
                {
                    arg->result = worker(arg->a, arg->b);
                }, &(args[i]));
            }
            for (size_t i = 0; i < num_threads; ++i)
            {
                args[i]._thread.join();
                printf("%zu\n", args[i].result);
            }
        }
    } break;
    case 2:
    {
        bool run = true;
        for (size_t i = 0; i < num_threads; ++i)
        {
            args[i]._thread = std::thread([&run](WorkerArgs* arg)
            {
                while (run)
                {
                    arg->compute.wait();
                    if (run)
                    {
                        arg->result = worker(arg->a, arg->b);
                        arg->done.set();
                    }
                    else
                        break;
                }
            }, &(args[i]));
        }

        size_t a = 0;
        while (a < n)
        {
            for (size_t i = 0; i < num_threads; ++i, a += chunks)
            {
                args[i].a = a;
                args[i].b = a + chunks;
                args[i].compute.set();
            }
            for (size_t i = 0; i < num_threads; ++i)
            {
                args[i].done.wait();   
            }
            for (size_t i = 0; i < num_threads; ++i)
            {
                printf("%zu\n", args[i].result);
            }
        }
        run = false;
        for (size_t i = 0; i < num_threads; ++i)
        {
            args[i].compute.set();
            args[i]._thread.join();
        }
    } break;
    default:
    {
        for (size_t i = 0; i < n; i += chunks)
            printf("%zu\n", worker(i, i + chunks));
    } break;
    }
}

int main(int argc, const char* argv[])
{
    size_t n = 20;
    size_t chunks = 16;
    size_t threads = 1;
    int thread_tactic = 1;

    arg::Parser parser("Test tool for Event and Thread performance.");

    parser.AddArg(n, { "-n" }, "logarithm of number of elements to compute, total elements = 2^n");
    parser.AddArg(chunks, { "-c" }, "logarithm of size of a chunk");
    parser.AddArg(threads, { "-t", "--thread", "--threads" }, "number of threads");
    parser.AddArg(thread_tactic, { "--type", "--tactic" }, "type of threading"
        "\n\t\t0: simple sequential calculation on the main thread"
        "\n\t\t1: process chunks in parallel, start a new bunch of threads each time."
        "\n\t\t2: process chunks in parallel, keep threads alive, signal threads when needed."
        , "", { 0, 1, 2 });

    parser.Do(argc, argv);

    if (!threads)
    {
        std::cerr << "number of threads should be positive!" << std::endl;
        exit(1);
    }
    Test(thread_tactic, threads, (size_t)1 << n, (size_t)1 << chunks);

}