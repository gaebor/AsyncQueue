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
			@return time since last Tick, construction.
		*/
        double Tock();
        ~Clock();
    private:
        typedef std::chrono::steady_clock MyClock;
        typedef MyClock::duration MyDuration;
		MyClock::time_point _timePoint;
        static const double _frequency;
        static double GetFrequency();
    };
}

#endif
