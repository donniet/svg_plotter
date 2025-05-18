/**
 * lower_bound method implements a binary search between the 
 * given pointers and returns a pointer to the first element which is
 * not less than the given value, as decided by the LOWER_BOUND_LESS_THAN
 * definition or, failing that, the < operator
 * REQUIRED: define LOWER_BOUND_TYPE to specify the type of the pointer
 * OPTIONAL: define LOWER_BOUND_LESS_THAN to specify the method used
 *           to compare the types in the array
 */

#ifndef LOWER_BOUND_LESS_THAN
#define LOWER_BOUND_LESS_THAN(a,b) ((a)<(b))
#endif

#ifndef LOWER_BOUND_TYPE
#define LOWER_BOUND_TYPE float
#endif

LOWER_BOUND_TYPE * lower_bound(
    LOWER_BOUND_TYPE * begin, LOWER_BOUND_TYPE * end, 
    LOWER_BOUND_TYPE value)
{
    while(begin < end)
    {
        let mid = (begin + end) >> 1;

        if(LOWER_BOUND_LESS_THAN(value, *mid))
        {
            end = mid;
            continue;
        }

        if(begin == mid)
            break;

        begin = mid;
    }

    return end;
}

#undef LOWER_BOUND_LESS_THAN
#undef LOWER_BOUND_TYPE

