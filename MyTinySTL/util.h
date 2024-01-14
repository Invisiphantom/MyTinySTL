#pragma once

#include <cstddef>

#include "type_traits.h"

namespace mystl {

/*----move----*/

template <typename T>
typename std::remove_reference<T>::type&& move(T&& arg) noexcept {
    return static_cast<typename std::remove_reference<T>::type&&>(arg);
}

/*----forward----*/

// lvalue
template <typename T>
constexpr T&& forward(
    typename std::remove_reference<T>::type& arg) noexcept {
    return static_cast<T&&>(arg);
}

// rvalue
template <typename T>
constexpr T&& forward(
    typename std::remove_reference<T>::type&& arg) noexcept {
    static_assert(!std::is_lvalue_reference<T>::value, "bad forward");
    return static_cast<T&&>(arg);
}

/*----swap----*/

template <typename Tp>
void swap(Tp& lhs, Tp& rhs) {
    auto tmp(mystl::move(lhs));
    lhs = mystl::move(rhs);
    rhs = mystl::move(tmp);
}

template <typename ForwardIter1, typename ForwardIter2>
ForwardIter2 swap_range(ForwardIter1 first1, ForwardIter1 last1,
                        ForwardIter2 first2) {
    for (; first1 != last1; ++first1, (void)++first2)
        mystl::swap(*first1, *first2);
    return first2;
}

template <typename Tp, size_t Nm>
void swap(Tp (&a)[Nm], Tp (&b)[Nm]) {
    mystl::swap_range(a, a + Nm, b);
}

/*--------pair--------*/

template <typename T1, typename T2>
struct pair {
    typedef T1 first_type;
    typedef T2 second_type;

    first_type first;
    second_type second;

    constexpr pair(const pair& rhs) = default;
    constexpr pair(pair&& rhs) = default;

    // Default constructor
    constexpr explicit(!std::__is_implicitly_default_constructible<T1>() ||
                       !std::__is_implicitly_default_constructible<T2>())
        pair()
        requires(std::is_default_constructible_v<T1> &&
                 std::is_default_constructible_v<T2>)
        : first(), second() {}

    /*--------------------------------------------------*/

   private:
    template <typename U1, typename U2>
    static constexpr bool ConstructiblePair() {
        return std::is_constructible<T1, U1>() &&
               std::is_constructible<T2, U2>();
    }

    template <typename U1, typename U2>
    static constexpr bool ConvertiblePair() {
        return std::is_convertible<U1, T1>() &&
               std::is_convertible<U2, T2>();
    }

    /*--------------------------------------------------*/

   public:
    /*----pair(U1&& x, U2&& y)----*/
    template <typename U1, typename U2>
        requires(ConstructiblePair<U1, U2>())
    constexpr explicit(!ConvertiblePair<U1, U2>()) pair(U1&& x, U2&& y)
        : first(mystl::forward<U1>(x)), second(mystl::forward<U2>(y)) {}

    /*----pair(const pair<U1, U2>& p)----*/
    template <class U1, class U2>
        requires(ConstructiblePair<const U1&, const U2&>())
    constexpr explicit(!ConvertiblePair<const U1&, const U2&>())
        pair(const pair<U1, U2>& p)
        : first(p.first), second(p.second) {}

    /*----pair(pair<U1, U2>&& p)----*/
    template <typename U1, class U2>
        requires(ConstructiblePair<const U1&, const U2&>())
    constexpr explicit(!ConvertiblePair<const U1&, const U2&>())
        pair(pair<U1, U2>&& p)
        : first(mystl::forward<U1>(p.first)),
          second(mystl::forward<U2>(p.second)) {}

    /*--------------------------------------------------*/

   private:
    template <typename U1, typename U2>
    static constexpr bool AssignablePair() {
        return std::is_assignable<T1&, U1>() &&
               std::is_assignable<T2&, U2>();
    }

    /*--------------------------------------------------*/

   public:
    /// Copy assignment operator
    constexpr pair& operator=(const pair& rhs)
        requires(AssignablePair<const T1&, const T2&>())
    {
        first = rhs.first;
        second = rhs.second;
        return *this;
    }

    /// Move assignment operator
    constexpr pair& operator=(pair&& rhs)
        requires(AssignablePair<T1, T2>())
    {
        first = forward<first_type>(rhs.first);
        second = forward<second_type>(rhs.second);
        return *this;
    }

    /// Copy assignment operator for other pair
    template <typename U1, class U2>
    constexpr pair& operator=(const pair<U1, U2>& other)
        requires(AssignablePair<const T1&, const T2&>())
    {
        first = other.first;
        second = other.second;
        return *this;
    }

    // Move assignment operator for other pair
    template <typename U1, class U2>
    constexpr pair& operator=(pair<U1, U2>&& other)
        requires(AssignablePair<T1, T2>())
    {
        first = mystl::forward<U1>(other.first);
        second = mystl::forward<U2>(other.second);
        return *this;
    }

    void swap(pair& other) {
        mystl::swap(first, other.first);
        mystl::swap(second, other.second);
    }
};

/*--------compare operator--------*/

template <typename T1, typename T2>
bool operator==(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs) {
    return lhs.first == rhs.first && lhs.second == rhs.second;
}

template <typename T1, typename T2>
bool operator<(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs) {
    return lhs.first < rhs.first ||
           (lhs.first == rhs.first && lhs.second < rhs.second);
}

template <typename T1, typename T2>
bool operator!=(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs) {
    return !(lhs == rhs);
}

template <typename T1, typename T2>
bool operator>(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs) {
    return rhs < lhs;
}

template <typename T1, typename T2>
bool operator<=(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs) {
    return !(rhs < lhs);
}

template <typename T1, typename T2>
bool operator>=(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs) {
    return !(lhs < rhs);
}

/*----swap(pair&, pair&)----*/
template <typename T1, typename T2>
void swap(pair<T1, T2>& lhs, pair<T1, T2>& rhs) {
    lhs.swap(rhs);
}

/*----make_pair(T1&&, T2&&)----*/
template <typename T1, typename T2>
constexpr pair<T1, T2> make_pair(T1&& first, T2&& second) {
    return pair<T1, T2>(mystl::forward<T1>(first),
                        mystl::forward<T2>(second));
}

}  // namespace mystl
