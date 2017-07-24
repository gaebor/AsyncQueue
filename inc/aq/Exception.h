#ifndef INCLUDE_AQ_Exception_H
#define INCLUDE_AQ_Exception_H

#include <exception>

//! namespace AsyncQueue
namespace aq {

    class Exception : public std::exception
    {
        const char* what_;
    public:
        Exception(const char* str);
        virtual const char* what() const throw ();
    };

}

#endif 
