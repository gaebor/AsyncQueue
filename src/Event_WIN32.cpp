
//          Copyright Gábor Borbély 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "aq/Event_WIN32.h"

namespace aq {
    EventImpl::EventImpl(EventTypeImpl type)
    {
        _event = CreateEvent(NULL, type == EVENT_AUTORESET_IMPL ? FALSE : TRUE, FALSE, NULL);
        if (!_event)
            throw Exception("cannot create event");
    }

    EventImpl::~EventImpl()
    {
        CloseHandle(_event);
    }


    void EventImpl::waitImpl()
    {
        switch (WaitForSingleObject(_event, INFINITE))
        {
        case WAIT_OBJECT_0:
            return;
        default:
            throw Exception("wait for event failed");
        }
    }

    bool EventImpl::waitImpl(long milliseconds)
    {
        switch (WaitForSingleObject(_event, milliseconds + 1))
        {
        case WAIT_TIMEOUT:
            return false;
        case WAIT_OBJECT_0:
            return true;
        default:
            throw Exception("wait for event failed");
        }
    }

}
