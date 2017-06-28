
//          Copyright Gábor Borbély 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef Event_WIN32_INCLUDED
#define Event_WIN32_INCLUDED

#include<windows.h>

class EventImpl
{
public:
	enum EventTypeImpl
	{
		EVENT_MANUALRESET_IMPL,
		EVENT_AUTORESET_IMPL,
	};

protected:
	EventImpl(EventTypeImpl type);
	~EventImpl();
	void setImpl();
	void waitImpl();
	bool waitImpl(long milliseconds);
	void resetImpl();
	
private:
	HANDLE _event;
};


//
// inlines
//
inline void EventImpl::setImpl()
{
	if (!SetEvent(_event))
	{
		throw EventException("cannot signal event");
	}
}

inline void EventImpl::resetImpl()
{
	if (!ResetEvent(_event))
	{
		throw EventException("cannot reset event");
	}
}

#endif // Event_WIN32_INCLUDED
