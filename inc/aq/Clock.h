#ifndef INCLUDE_AQ_Clock_H
#define INCLUDE_AQ_Clock_H

#include <chrono>

namespace aq {

    class Clock
    {
    public:
        Clock();
        void Tick();
        double Tock();
        ~Clock();
    private:
        typedef std::chrono::high_resolution_clock MyClock;
        typedef MyClock::duration MyDuration;
        std::chrono::time_point<MyClock, MyDuration> _timePoint;
        static const double _frequency;
        static double GetFrequency();
    };

}

#endif
