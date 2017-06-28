
//          Copyright Gábor Borbély 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef Event_INCLUDED
#define Event_INCLUDED

#include <exception>

class EventException : public std::exception
{
    const char* what_;
public:
    EventException(const char* str) : what_(str){}
    const char* what() const throw () {return what_;}
};

#ifdef _MSC_VER
#include "Event_WIN32.h"
#else
#include "Event_POSIX.h"
#endif

class Event: private EventImpl
	/// An Event is a synchronization object that
	/// allows one thread to signal one or more
	/// other threads that a certain event
	/// has happened.
	/// Usually, one thread signals an event,
	/// while one or more other threads wait
	/// for an event to become signalled.
{
public:
	enum EventType
	{
		EVENT_MANUALRESET = EVENT_MANUALRESET_IMPL, /// Manual reset event
		EVENT_AUTORESET = EVENT_AUTORESET_IMPL      /// Auto-reset event
	};

	explicit Event(EventType type = EVENT_AUTORESET);
		/// Creates the event. If type is EVENT_AUTORESET,
		/// the event is automatically reset after
		/// a wait() successfully returns.

	//@ deprecated
	explicit Event(bool autoReset);
		/// Please use Event::Event(EventType) instead.

	~Event();
		/// Destroys the event.

	void set();
		/// Signals the event. If autoReset is true,
		/// only one thread waiting for the event 
		/// can resume execution.
		/// If autoReset is false, all waiting threads
		/// can resume execution.

	void wait();
		/// Waits for the event to become signalled.

	void wait(long milliseconds);
		/// Waits for the event to become signalled.
		/// Throws a TimeoutException if the event
		/// does not become signalled within the specified
		/// time interval.

	bool tryWait(long milliseconds);
		/// Waits for the event to become signalled.
		/// Returns true if the event
		/// became signalled within the specified
		/// time interval, false otherwise.

	void reset();
		/// Resets the event to unsignalled state.
	
private:
	Event(const Event&);
	Event& operator = (const Event&);
};


//
// inlines
//
inline void Event::set()
{
	setImpl();
}


inline void Event::wait()
{
	waitImpl();
}


inline void Event::wait(long milliseconds)
{
	if (!waitImpl(milliseconds))
		throw EventException("timeout!");
}


inline bool Event::tryWait(long milliseconds)
{
	return waitImpl(milliseconds);
}


inline void Event::reset()
{
	resetImpl();
}


#endif // Event_INCLUDED
