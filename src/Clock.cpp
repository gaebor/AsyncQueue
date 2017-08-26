#include "aq/Clock.h"

namespace aq {

    Clock::Clock()
    {
        Tick();
    }

    void Clock::Tick()
    {
        _timePoint = MyClock::now();
    }

    const double Clock::_frequency = GetFrequency();

    double Clock::GetFrequency()
    {
        return (double)Clock::MyDuration::period::num / Clock::MyDuration::period::den;
    }

    double Clock::Tock()
    {
        auto const now = MyClock::now();
        const auto elapsed = _frequency * (now - _timePoint).count();
        return elapsed;
    }

    Clock::~Clock()
    {
    }

}
