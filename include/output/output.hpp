#ifndef __OUTPUT_HPP__
#define __OUTPUT_HPP__

#include <utility>
#include <iostream>

using std::pair;
using std::size_t;
using std::ostream;


struct Outputer
{
    // perhaps remove the const?
    virtual void print(ostream & os) const = 0;
};

ostream & operator<<(ostream & os, Outputer const & o)
{
    o.print(os);
    return os;
}

#endif