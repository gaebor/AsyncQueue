#ifndef INCLUDE_AQ_AsyncQueue_H
#define INCLUDE_AQ_AsyncQueue_H

#include <mutex>

#include <queue>
#include <limits>
#include <atomic>

#include "aq/Event.h"

//! namespace AsyncQueue
namespace aq {
    
    //! you can control the behavior of the overloaded queue
    enum LimitBehavior : int
    {
        None = 0, //!< the queue can grow as much as it can, queueLimit is irrelevant
        Drop = 1, //!< drop elements if queue size is above the given limit
        Wait = 2, //!< wait until the queue size drops below the given limit
        Refuse = 3, //!< refuse to enqueue elements if queue size is above the given limit
    };
    
    //!simple producer/consumer tool
    /*!
        the queue can be fed and consumed by many threads
        @param _Ty stored type, it have to be copyable
        @param SeeHighWater determine whether you want to see the high water
    */
    template<class _Ty, bool SeeHighWater = false, typename _Container = std::queue<_Ty>>
    class AsyncQueue
    {
    private:
        typedef std::lock_guard<std::mutex> AutoLock;
    public:
        typedef _Container Container;
        //! specifies limit behavior
        /*!
            it can be adjusted at any time (thread safe)
            invalid value is treated as None
        */
        std::atomic<LimitBehavior> limitBehavior;
        //! the size limit of the queue
        /*!
            it can be adjusted at any time (thread safe)

            If the queueLimit is zero and the limitBehavior is Refuse then every EnQueue fails
            If the queueLimit is zero and the limitBehavior is Wait then you can still enqueue one element
            If the queueLimit is zero and the limitBehavior is Drop then every EnQueue empties the queue, but you can enqueue one element
            If the limitBehavior is None then queueLimit is irrelevant
        */
        std::atomic<size_t> queueLimit;

        AsyncQueue(LimitBehavior l = None, size_t limit = std::numeric_limits<size_t>::max())
            : limitBehavior(l),
            queueLimit(limit),
            _highWater(0),
            _content(),
            _empty(),
            _belowLimit()
        {
            _belowLimit.set();
            _empty.set();
            _content.reset();
        }
        ~AsyncQueue(void)
        {
        }

        //!clears and resets the queue like it would be a new one
        void Reset()
        {
            WakeUp();
            _content.reset();
            _highWater = 0;
        }

        //!puts an item into the queue
        /*!
            if the behavior is Wait, then the call locks until the queue is small enough.
            if the behavior is Drop and the queue is long, then the entire queue is dropped and the new one is enqueued
            if the EnQueue is in a locking state and a WakeUp is called, then the enqueue fails and the functions returns
        */
        bool EnQueue(const _Ty& element)
        {
			switch (limitBehavior)
			{
			case LimitBehavior::Drop:
				if (_queue.size() >= queueLimit)
				{
					AutoLock lock(_mutex);
					while (!_queue.empty())
						_queue.pop();
					EnQueue_internal(element);
					return true;
				}
                else
                {
                    AutoLock lock(_mutex);
                    EnQueue_internal(element);
                    return true;
                }
			case LimitBehavior::Refuse:
				if (_queue.size() >= queueLimit)
					return false;
				else
				{
					AutoLock lock(_mutex);
					EnQueue_internal(element);
					return true;
				}
			case LimitBehavior::Wait:
                _belowLimit.wait();
			default:
			{
				AutoLock lock(_mutex);
				EnQueue_internal(element);
				return true;
			}
			}
        }

        //! extract one item from the queue
        /*!
            the call locks until there is something to extract from the queue or a WakeUp is called.
            @return the success of the Dequeuing, false if the WakeUp is called meanwhile
        */
        bool DeQueue(_Ty& element)
        {
            _content.wait();
            return DeQueue_internal(element);
        }

        void WaitForEmpty()
        {
            _empty.wait();
        }

   //     bool DeQueue(_Ty& element, long miliseconds)
   //     {
			//return _content.tryWait(miliseconds) && DeQueue_internal(element);
   //     }

        //!this causes the DeQueue to return false if the queue was empty, otherwise nothing happens
        void WakeUpIfEmpty()
        {
            _content.set(); //even if it is empty
        }
        //!this drops all the elements in the waiting queue and then wakes up
        /*!
            This cause the DeQueue to return false and terminates the pending EnQueue calls if those ware waiting for something
            @return the number of dropped elements
        */
        size_t WakeUp()
        {
            AutoLock lock(_mutex);
            const auto result = _queue.size();
            while (!_queue.empty())
                _queue.pop();
            _belowLimit.set();
            _content.set();
            _empty.set();
            return result;
        }
        //! does not lock, therefore result is only an estimate
        size_t GetSize() const { return _queue.size(); }
        //! does not lock
        size_t GetHighWater() const { return _highWater; }
    private:
		void EnQueue_internal(const _Ty& element)
		{
			_queue.push(element);
			if (SeeHighWater)
				_highWater = std::max(_queue.size(), _highWater);
			_content.set();
			_empty.reset();
			if (_queue.size() >= queueLimit)
				_belowLimit.reset();
		}
        bool DeQueue_internal(_Ty& element)
        {
            AutoLock lock(_mutex);
            if (_queue.empty())
            {
				_content.reset();
                _empty.set();
                _belowLimit.set();
                return false;
            }
            element = _queue.front();
            _queue.pop();
            if (_queue.empty())
            {
                _empty.set();
                _content.reset();
                _belowLimit.set();
            }
            else if (_queue.size() < queueLimit)
                _belowLimit.set();

            return true;
        }
        size_t _highWater;
        Event<false> _content, _empty, _belowLimit;
        std::mutex _mutex;
        Container _queue;
    };

}

#endif
