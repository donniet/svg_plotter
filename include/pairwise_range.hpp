#ifndef __PAIRWISE_RANGE_HPP__
#define __PAIRWISE_RANGE_HPP__


#include <utility>

using std::pair;

template<typename Iter>
struct pairwise_range 
{
    Iter _original_begin;
    Iter _original_end;
    bool closed;

    NextPointRange(Iter begin, Iter end, bool closed = true) :
        _original_begin(begin), _original_end(end)
    { }

    struct iterator 
    {
        Iter _original_begin;
        Iter _original_end;
        Iter _prev;

        using value_type = pair<typename Iter::value_type, typename Iter::value_type>;
        using difference_type = typename Iter::difference_type;

        iterator() : 
            _original_begin(), _original_end(_original_begin), _prev(_original_end) 
        { }

        iterator(iterator const & i) :
            _original_begin(i._original_begin), _original_end(i._original_end), _prev(i._prev)
        { }

        iterator(Iter original_begin, Iter original_end, Iter prev) :
            _original_begin(original_begin), _original_end(original_end), _prev(prev)
        { }

        bool operator==(iterator const & i) const
        {
            if(_prev == i._prev)
                return true;

            if(_prev == _original_end && i._prev == i._original_end)
                return true;

            return false;
        }
        bool operator!=(iterator const & i) const 
        {
            return !(*this == i);
        }
        iterator & operator=(iterator const & i)
        {
            _original_begin = i._original_begin;
            _original_end = i._original_end;
            _prev = i._prev;
            return *this;
        }
        iterator & operator++()
        {  
            if(_prev != _original_end)
                ++_prev;

            return *this; 
        }
        iterator operator++(int)
        {
            iterator r = *this;
            operator++();
            return r;
        }
        value_type operator*()
        {
            if(_prev == _original_end)
                return *_prev;

            Iter c = _prev;
            ++c;

            if(c == _original_end)
                return *_original_begin;

            return *c;
        }
        
    };

    iterator begin() const 
    {
        return iterator(_original_begin, _original_end, _original_begin);
    }
    iterator end() const
    {
        return iterator(_original_begin, _original_end, _original_end);
    }
};


#endif