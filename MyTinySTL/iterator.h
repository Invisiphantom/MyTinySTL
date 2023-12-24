#pragma once

#include <cstddef>

#include "type_traits.h"

namespace mystl {

/*----iterator_tag----*/

struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag : public input_iterator_tag {};
struct bidirectional_iterator_tag : public forward_iterator_tag {};
struct random_access_iterator_tag : public bidirectional_iterator_tag {};

/*--------iterator--------*/

template <typename Category, typename T, typename Distance = ptrdiff_t, typename Pointer = T *,
          typename Reference = T &>
struct iterator {
    typedef Category iterator_category;
    typedef T value_type;
    typedef Pointer pointer;
    typedef Reference reference;
    typedef Distance difference_type;
};

template <typename T>
struct has_iterator_cat {
   private:
    struct two {
        char a;
        char b;
    };
    // 如果匹配失败，返回 two
    template <typename U>
    static two test(...);
    // 如果匹配成功，返回 char
    template <typename U>
    static char test(typename U::iterator_category * = 0);

   public:
    // 判断是否匹配成功
    static const bool value = (sizeof(test<T>(0)) == sizeof(char));
};

/*----iterator_traits----*/

/*
    iterator_traits
    |---iterator_traits_helper <is_convertible>
        |---iterator_traits_impl <has_iterator_cat>
*/

template <typename _Iterator, bool>
struct iterator_traits_impl {};

template <typename _Iterator>
struct iterator_traits_impl<_Iterator, true> {
    typedef typename _Iterator::iterator_category iterator_category;
    typedef typename _Iterator::value_type value_type;
    typedef typename _Iterator::pointer pointer;
    typedef typename _Iterator::reference reference;
    typedef typename _Iterator::difference_type difference_type;
};

template <typename _Iterator, bool>
struct iterator_traits_helper {};

template <typename _Iterator>
struct iterator_traits_helper<_Iterator, true>
    : public iterator_traits_impl<
          _Iterator, std::is_convertible<typename _Iterator::iterator_category, input_iterator_tag>::value ||
                         std::is_convertible<typename _Iterator::iterator_category, output_iterator_tag>::value> {};

template <typename _Iterator>
struct iterator_traits : public iterator_traits_helper<_Iterator, has_iterator_cat<_Iterator>::value> {};

// 针对原生指针的偏特化版本
template <typename T>
struct iterator_traits<T *> {
    typedef random_access_iterator_tag iterator_category;
    typedef T value_type;
    typedef T *pointer;
    typedef T &reference;
    typedef ptrdiff_t difference_type;
};

// 针对 const 原生指针的偏特化版本
template <typename T>
struct iterator_traits<const T *> {
    typedef random_access_iterator_tag iterator_category;
    typedef T value_type;
    typedef const T *pointer;
    typedef const T &reference;
    typedef ptrdiff_t difference_type;
};

/*
    has_iterator_cat_of
    |---has_iterator_cat && is_convertible <iterator_traits, U>
*/

template <typename T, typename U, bool = has_iterator_cat<iterator_traits<T>>::value>
struct has_iterator_cat_of
    : public m_bool_constant<std::is_convertible<typename iterator_traits<T>::iterator_category, U>::value> {};

template <typename T, class U>
struct has_iterator_cat_of<T, U, false> : public m_false_type {};

template <typename Iter>
struct is_input_iterator : public has_iterator_cat_of<Iter, input_iterator_tag> {};

template <typename Iter>
struct is_output_iterator : public has_iterator_cat_of<Iter, output_iterator_tag> {};

template <typename Iter>
struct is_forward_iterator : public has_iterator_cat_of<Iter, forward_iterator_tag> {};

template <typename Iter>
struct is_bidirectional_iterator : public has_iterator_cat_of<Iter, bidirectional_iterator_tag> {};

template <typename Iter>
struct is_random_access_iterator : public has_iterator_cat_of<Iter, random_access_iterator_tag> {};

template <typename _Iterator>
struct is_iterator
    : public m_bool_constant<is_input_iterator<_Iterator>::value || is_output_iterator<_Iterator>::value> {};

/*--------------------------------------------------*/

// iterator_category
template <typename _Iterator>
typename iterator_traits<_Iterator>::iterator_category iterator_category(const _Iterator &) {
    typedef typename iterator_traits<_Iterator>::iterator_category Category;
    return Category();
}

// distance_type*
template <typename _Iterator>
typename iterator_traits<_Iterator>::difference_type *distance_type(const _Iterator &) {
    return static_cast<typename iterator_traits<_Iterator>::difference_type *>(0);
}

// value_type*
template <typename _Iterator>
typename iterator_traits<_Iterator>::value_type *value_type(const _Iterator &) {
    return static_cast<typename iterator_traits<_Iterator>::value_type *>(0);
}

/*----distance----*/

// input_iterator_tag
template <typename InputIterator>
typename iterator_traits<InputIterator>::difference_type distance_dispatch(InputIterator first, InputIterator last,
                                                                           input_iterator_tag) {
    typename iterator_traits<InputIterator>::difference_type n = 0;
    while (first != last) {
        ++first;
        ++n;
    }
    return n;
}

// random_access_iterator_tag
template <typename RandomIter>
typename iterator_traits<RandomIter>::difference_type distance_dispatch(RandomIter first, RandomIter last,
                                                                        random_access_iterator_tag) {
    return last - first;
}

template <typename InputIterator>
typename iterator_traits<InputIterator>::difference_type distance(InputIterator first, InputIterator last) {
    return distance_dispatch(first, last, iterator_category(first));
}

/*----advance----*/

// input_iterator_tag
template <typename InputIterator, class Distance>
void advance_dispatch(InputIterator &i, Distance n, input_iterator_tag) {
    while (n--) ++i;
}

// bidirectional_iterator_tag
template <typename BidirectionalIterator, class Distance>
void advance_dispatch(BidirectionalIterator &i, Distance n, bidirectional_iterator_tag) {
    if (n >= 0)
        while (n--) ++i;
    else
        while (n++) --i;
}

// random_access_iterator_tag
template <typename RandomIter, class Distance>
void advance_dispatch(RandomIter &i, Distance n, random_access_iterator_tag) {
    i += n;
}

template <typename InputIterator, class Distance>
void advance(InputIterator &i, Distance n) {
    advance_dispatch(i, n, iterator_category(i));
}

/*--------reverse_iterator--------*/

template <typename _Iterator>
class reverse_iterator {
   private:
    _Iterator current;

   public:
    typedef typename iterator_traits<_Iterator>::iterator_category iterator_category;
    typedef typename iterator_traits<_Iterator>::value_type value_type;
    typedef typename iterator_traits<_Iterator>::difference_type difference_type;
    typedef typename iterator_traits<_Iterator>::pointer pointer;
    typedef typename iterator_traits<_Iterator>::reference reference;

   public:
    reverse_iterator() {}
    explicit reverse_iterator(_Iterator iter) : current(iter) {}
    reverse_iterator(const reverse_iterator &rhs) : current(rhs.current) {}

   public:
    _Iterator base() const { return current; }

    // *iter
    reference operator*() const {
        _Iterator tmp = current;
        return *--tmp;
    }

    // iter->mem
    pointer operator->() const { return &(operator*()); }

    // ++iter
    reverse_iterator &operator++() {
        --current;
        return *this;
    }

    // iter++
    reverse_iterator operator++(int) {
        reverse_iterator tmp = *this;
        --current;
        return tmp;
    }

    // --iter
    reverse_iterator &operator--() {
        ++current;
        return *this;
    }

    // iter--
    reverse_iterator operator--(int) {
        reverse_iterator tmp = *this;
        ++current;
        return tmp;
    }

    // iter += n
    reverse_iterator &operator+=(difference_type n) {
        current -= n;
        return *this;
    }

    // iter + n
    reverse_iterator operator+(difference_type n) const { return reverse_iterator(current - n); }

    // iter -= n
    reverse_iterator &operator-=(difference_type n) {
        current += n;
        return *this;
    }

    // iter - n
    reverse_iterator operator-(difference_type n) const { return reverse_iterator(current + n); }

    // iter[n]
    reference operator[](difference_type n) const { return *(*this + n); }
};

// lhs - rhs
template <typename _Iterator>
typename reverse_iterator<_Iterator>::difference_type operator-(const reverse_iterator<_Iterator> &lhs,
                                                                const reverse_iterator<_Iterator> &rhs) {
    return rhs.base() - lhs.base();
}

// lhs == rhs
template <typename _Iterator>
bool operator==(const reverse_iterator<_Iterator> &lhs, const reverse_iterator<_Iterator> &rhs) {
    return lhs.base() == rhs.base();
}

// lhs < rhs
template <typename _Iterator>
bool operator<(const reverse_iterator<_Iterator> &lhs, const reverse_iterator<_Iterator> &rhs) {
    return rhs.base() < lhs.base();
}

// lhs != rhs
template <typename _Iterator>
bool operator!=(const reverse_iterator<_Iterator> &lhs, const reverse_iterator<_Iterator> &rhs) {
    return !(lhs == rhs);
}

// lhs > rhs
template <typename _Iterator>
bool operator>(const reverse_iterator<_Iterator> &lhs, const reverse_iterator<_Iterator> &rhs) {
    return rhs < lhs;
}

// lhs <= rhs
template <typename _Iterator>
bool operator<=(const reverse_iterator<_Iterator> &lhs, const reverse_iterator<_Iterator> &rhs) {
    return !(rhs < lhs);
}

// lhs >= rhs
template <typename _Iterator>
bool operator>=(const reverse_iterator<_Iterator> &lhs, const reverse_iterator<_Iterator> &rhs) {
    return !(lhs < rhs);
}

}  // namespace mystl
