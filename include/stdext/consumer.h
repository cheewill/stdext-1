#ifndef STDEXT_CONSUMER_INCLUDED
#define STDEXT_CONSUMER_INCLUDED
#pragma once

#include "iterator.h"


namespace stdext
{
    template <class T> struct is_consumer : ::std::false_type { };
    template <class T, class Elem>
    struct is_consumer<T(Elem)> : is_callable<T(Elem), bool> { };

    // I would love to add is_consumer_adaptable, as_consumer, and can_consume, but I can't
    // because there's no way to force is_consumer_adaptable to see make_consumer<Elem> as
    // a dependent function template.

    template <class Iterator>
    class iterator_consumer
    {
    public:
        using iterator = Iterator;

    public:
        iterator_consumer() : i() { }
        explicit iterator_consumer(const iterator& i) : i(i) { }
        explicit iterator_consumer(iterator&& i) : i(::std::move(i)) { }

    public:
        template <class T>
        bool operator () (T&& value)
        {
            *i++ = ::std::forward<T>(value);
            return true;
        }

    private:
        iterator i;
    };

    template <class Iterator, class Sentinel>
    class delimited_iterator_consumer
    {
    public:
        using iterator = Iterator;
        using sentinel = Sentinel;

    public:
        delimited_iterator_consumer() : i(), j() { }
        delimited_iterator_consumer(const iterator& i, const sentinel& j) : i(i), j(j) { }
        delimited_iterator_consumer(const iterator& i, sentinel&& j) : i(i), j(::std::move(j)) { }
        delimited_iterator_consumer(iterator&& i, const sentinel& j) : i(::std::move(i)), j(j) { }
        delimited_iterator_consumer(iterator&& i, sentinel&& j) : i(::std::move(i)), j(::std::move(j)) { }

    public:
        template <class T>
        bool operator () (T&& value)
        {
            if (i == j)
                return false;
            *i++ = ::std::forward<T>(value);
            return true;
        }

    private:
        Iterator i;
        Sentinel j;
    };

    template <class Elem, class Iterator,
        REQUIRES(::std::is_convertible<Elem, value_type<::std::decay_t<Iterator>, is_iterator>>::value)>
    auto make_consumer(Iterator&& i)
    {
        return iterator_consumer<::std::decay_t<Iterator>>(::std::forward<Iterator>(i));
    }

    template <class Elem, class Iterator, class Sentinel,
        REQUIRES(::std::is_convertible<Elem, value_type<::std::decay_t<Iterator>, is_iterator>>::value
            && is_equality_comparable<::std::decay_t<Iterator>, ::std::decay_t<Sentinel>>::value)>
    auto make_consumer(Iterator&& i, Sentinel&& j)
    {
        return delimited_iterator_consumer<::std::decay_t<Iterator>, ::std::decay_t<Sentinel>>(::std::forward<Iterator>(i), ::std::forward<Sentinel>(j));
    }
}

#endif