#pragma once

#include <new>

#include "iterator.h"
#include "type_traits.h"

namespace mystl {

/*----construct----*/

template <typename Tp>
void construct(Tp* ptr) {
    ::new ((void*)ptr) Tp();
}

template <typename Tp, typename... Args>
void construct(Tp* ptr, Args&&... args) {
    ::new ((void*)ptr) Tp(mystl::forward<Args>(args)...);
}

/*----destroy----*/

template <typename Tp>
void destroy_one(Tp*, std::true_type) {}

template <typename Tp>
void destroy_one(Tp* pointer, std::false_type) {
    if (pointer != nullptr) {
        pointer->~Tp();
    }
}

template <typename ForwardIter>
void destroy_cat(ForwardIter, ForwardIter, std::true_type) {}

template <typename ForwardIter>
void destroy_cat(ForwardIter first, ForwardIter last, std::false_type) {
    for (; first != last; ++first) destroy(&*first);
}

template <typename Tp>
void destroy(Tp* pointer) {
    destroy_one(pointer, std::is_trivially_destructible<Tp>{});
}

template <typename ForwardIter>
void destroy(ForwardIter first, ForwardIter last) {
    destroy_cat(first, last, std::is_trivially_destructible<typename iterator_traits<ForwardIter>::value_type>{});
}

}  // namespace mystl
