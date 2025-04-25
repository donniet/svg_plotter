
#include "output/output.hpp"

using std::ostream;

ostream & operator<<(ostream & os, Outputer const & o)
{
    o.print(os);
    return os;
}
