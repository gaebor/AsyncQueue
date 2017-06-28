
//          Copyright Gábor Borbély 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef Event_POSIX_INCLUDED
#define Event_POSIX_INCLUDED

#include <pthread.h>
#include <errno.h>

class EventImpl
{
public:
	enum EventTypeImpl
	{
		EVENT_MANUALRESET_IMPL,
		EVENT_AUTORESET_IMPL
	};

protected:
	EventImpl(EventTypeImpl type);
	~EventImpl();
	void setImpl();
	void waitImpl();
	bool waitImpl(long milliseconds);
	void resetImpl();
	
private:
	bool            _auto;
	volatile bool   _state;
	pthread_mutex_t _mutex;
	pthread_cond_t  _cond;
};


//
// inlines
//
inline void EventImpl::setImpl()
{
	if (pthread_mutex_lock(&_mutex))	
		throw EventException("cannot signal event (lock)");
	_state = true;
	if (pthread_cond_broadcast(&_cond))
	{
		pthread_mutex_unlock(&_mutex);
		throw EventException("cannot signal event");
	}
	pthread_mutex_unlock(&_mutex);
}


inline void EventImpl::resetImpl()
{
	if (pthread_mutex_lock(&_mutex))	
		throw EventException("cannot reset event");
	_state = false;
	pthread_mutex_unlock(&_mutex);
}

#endif // Event_POSIX_INCLUDED
