#ifndef INCLUDE_AQ_Clock_H
#define INCLUDE_AQ_Clock_H

#include <chrono>

namespace aq {

	//! simple class to measure time, not thread safe!
    class Clock
    {
    public:
		//! marks instantiation time
        Clock();
		//! marks current time
        void Tick();
		//! returns time between former time mark and now
		/*!
			@return time since last Tick, Tock or construction.
			Also marks the current time (implies Tick)
		*/
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
