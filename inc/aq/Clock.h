#ifndef INCLUDE_AQ_Clock_H
#define INCLUDE_AQ_Clock_H

#include <chrono>

namespace aq {

	//! simple class to measure time, not thread safe!
    template<class ClockTy = std::chrono::steady_clock>
    class Clock
    {
    public:
		//! marks instantiation time
        Clock()
        {
            Tick();
        }
		//! marks current time
        void Tick()
        {
            _timePoint = MyClock::now();
        }
		//! returns time between former time mark and now
		/*!
			@return time since last Tick, construction.
		*/
        double Tock()
        {
            auto const now = MyClock::now();
            const auto elapsed = _frequency * (now - _timePoint).count();
            return elapsed;
        }
        ~Clock(){}
    private:
        typedef ClockTy MyClock;
        typedef typename MyClock::duration MyDuration;
        typename MyClock::time_point _timePoint;
        static constexpr double _frequency = (double)MyDuration::period::num / MyDuration::period::den;
    };
}

#endif
