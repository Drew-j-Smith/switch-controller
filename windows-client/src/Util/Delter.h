#ifndef DELETER_H
#define DELETER_H

#include "pch.h"

template <class T> struct PointerDeleter {
    std::function<void(T *)> deleterFunction;
    PointerDeleter() {}
    PointerDeleter(std::function<void(T *)> f) : deleterFunction(f) {}
    void operator()(T *t) const noexcept { deleterFunction(t); }
};

template <class T>
using unique_ptr_w_deleter = std::unique_ptr<T, PointerDeleter<T>>;

template <class T> std::function<void(T *)> deref(std::function<void(T **)> f) {
    return [&f](T *t) { f(&t); };
}

#endif