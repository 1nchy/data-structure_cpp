#ifndef _ASP_ITERATOR_HPP_
#define _ASP_ITERATOR_HPP_

#include "basic_param.hpp"

namespace asp {
template <typename _Tp, typename _Pointer, typename _Reference> struct base_iterator;
template <typename _Tp, typename _Pointer = _Tp*, typename _Reference = _Tp&> struct base_iterator {
    // typedef typename _Tp value_type;
    // typedef typename _Pointer pointer;
    // typedef typename _Reference reference;
    using value_type = _Tp;
    using pointer = _Pointer;
    using reference = _Reference;
    typedef base_iterator<_Tp> self;

    base_iterator() = default;
    base_iterator(const pointer _p) : _ptr(_p) {}
    base_iterator(const self& _s) : _ptr(_s._ptr) {}
    self& operator=(const self& _s) {
        this->_ptr = _s._ptr;
    }

    reference operator*() {
        return *_ptr;
    }
    pointer operator->() {
        return _ptr;
    }
    const pointer& base() const {
        return _ptr;
    }

    friend bool operator==(const self& lhs, const self& rhs) {
        return lhs._ptr == rhs._ptr;
    }
    friend bool operator!=(const self& lhs, const self& rhs) {
        return lhs._ptr != rhs._ptr;
    }

    value_type* _ptr = nullptr;
};

template <typename _Tp> 
struct normal_iterator;

template <typename _Tp> 
struct normal_iterator : public base_iterator <_Tp> {
    // typedef value_type = base_iterator<_Tp>::value_type;
    // typedef pointer = base_iterator<_Tp>::pointer;
    // typedef reference = base_iterator<_Tp>::reference;
    typedef typename base_iterator<_Tp>::value_type value_type;
    typedef typename base_iterator<_Tp>::pointer pointer;
    typedef typename base_iterator<_Tp>::reference reference;
    typedef base_iterator<_Tp> base;
    typedef normal_iterator<value_type> self;

    using base_iterator<_Tp>::_ptr;

    normal_iterator() = default;
    normal_iterator(const pointer _p): base(_p) {}
    normal_iterator(const self& rhs): base(rhs._ptr) {}
    self& operator=(const self& rhs) {
        _ptr = rhs._ptr;
    }
    // @iterator to @const_iterator conversion
    // template <typename _Iter> normal_iterator(const normal_iterator<_Iter>& _i)
    //  : base(_i) {}
    
    self& operator++() {
        ++_ptr;
        return *this;
    }
    self operator++(int) {
        self _tmp = *this;
        ++_ptr;
        return _tmp;
    }
    self& operator--() {
        --_ptr;
        return *this;
    }
    self operator--(int) {
        self _tmp = *this;
        --_ptr;
        return _tmp;
    }
    self operator+(int _n) {
        self _tmp = *this;
        _tmp._ptr += _n;
        return _tmp;
    }
    self operator-(int _n) {
        self _tmp = *this;
        _tmp._ptr -= _n;
        return _tmp;
    }
    friend difference_type operator-(const self& last, const self& first) {
        return last._ptr - first._ptr;
    }
// private:
    // value_type* _ptr = nullptr;
};


template <typename _Iterator> difference_type distance(_Iterator first, _Iterator last);

template <typename _Iterator> difference_type distance(_Iterator first, _Iterator last) {
    difference_type _n = 0;
    while (first != last) {
        ++first;
        ++n;
    }
    return _n;
}
template <typename _Tp> difference_type distance(normal_iterator<_Tp> first, normal_iterator<_Tp> last) {
    return last - first;
}

};

#endif