#ifndef DELETER_H
#define DELETER_H

#include "pch.h"

template <class T, void (*F)(T *)> struct PointerDeleter {
    void operator()(T *t) const noexcept { F(t); }
};

template <class T, void (*F)(T *)>
using unique_ptr_w_deleter = std::unique_ptr<T, PointerDeleter<T, F>>;

template <class T, void (*F)(T **)> struct DoublePointerDeleter {
    void operator()(T *t) const noexcept { F(&t); }
};

template <class T, void (*F)(T **)>
using unique_dbl_ptr_w_deleter = std::unique_ptr<T, DoublePointerDeleter<T, F>>;

#endif