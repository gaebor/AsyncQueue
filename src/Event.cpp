
//          Copyright Gábor Borbély 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "aq/Event.h"

#if defined(_MSC_VER)
#include "Event_WIN32.cpp"
#else
#include "Event_POSIX.cpp"
#endif

namespace aq{

    Event::Event(EventType type): EventImpl((EventTypeImpl) type)
    {
    }


    Event::Event(bool autoReset): EventImpl(autoReset ? EVENT_AUTORESET_IMPL : EVENT_MANUALRESET_IMPL)
    {
    }


    Event::~Event()
    {
    }

} // namespace aq
