#ifndef  _ASP_DEQUE_HPP_
#define _ASP_DEQUE_HPP_

#include <ostream>

#include "deque_base.hpp"
#include "memory.hpp"

namespace asp {

// template <typename _Tp, typename _Alloc = std::allocator<_Tp>> class deque;

template <typename _Tp, typename _Alloc> class deque : public deque_base<_Tp, _Alloc> {
    typedef deque_base<_Tp, _Alloc> base;
public:
    typedef deque<_Tp, _Alloc> self;
    using value_type = typename base::value_type;
    using pointer = typename base::pointer;
    using reference = typename base::reference;
    using elt_pointer = typename base::elt_pointer;
    using map_pointer = typename base::map_pointer;
    using elt_allocator_type = typename base::elt_allocator_type;
    using map_allocator_type = typename base::map_allocator_type;

    using iterator = typename base::iterator;
    using const_iterator = typename base::const_iterator;

    using base::_data;

    deque() : base() {}
    explicit deque(const elt_allocator_type& _a) : base(_a) {}
    explicit deque(size_type _n, const elt_allocator_type& _a = elt_allocator_type())
     : base(_a, _n) {}
    deque(const self& _x) : base(_x._M_get_elt_allocator(), _x.size()) {
        this->_M_range_copy(_x.cbegin(), _x.cend(), this->_data._start);
    }
    deque(self&& _x) : base(std::move(_x)) {}
    ~deque() = default;

    self& operator=(const deque& _x);
    self& operator=(deque&& _x);


    /// element access
    reference operator[](size_type _i) {
        return *(this->_data._start + difference_type(_i));
    }
    const reference operator[](size_type _i) const {
        return *(this->_data._start + difference_type(_i));
    }
    reference front() { return *begin(); }
    const reference front() const { return *cbegin(); }
    reference back() {
        iterator _tmp = end();
        --_tmp;
        return *_tmp;
    }
    const reference back() const {
        const_iterator _tmp = end();
        --_tmp;
        return *_tmp;
    }


    /// iterator
    iterator begin() { return this->_data._start; }
    const_iterator cbegin() const { return this->_data._start; }
    iterator end() { return this->_data._finish; }
    const_iterator cend() const { return this->_data._finish; }


    /// capacity
    size_type size() const { return this->_data._finish - this->_data._start; }
    bool empty() const { return this->_data._finish == this->_data._start; }
    void shrink_to_fit();


    /// modifiers
    void push_back(const value_type& _x) {
        if (this->_data._finish._cur != this->_data._finish._last - 1) {
            this->_M_construct_node(this->_data._finish._cur, _x);
            ++this->_data._finish;
        }
        else {
            _M_push_back(_x);
        }
    }
    void push_back(value_type&& _x) {
        emplace_back(std::move(_x));
    }
    void pop_back() {
        if (empty()) { return; }
        if (this->_data._finish._cur != this->_data._finish._first) {
            --this->_data._finish._cur;
            _M_destroy_element(this->_data._finish);
        }
        else {
            _M_pop_back();
        }
    }
    void push_front(const value_type& _x) {
        if (this->_data._start._cur != this->_data._start._first) {
            --this->_data._start;
            this->_M_construct_node(this->_data._start._cur, _x);
        }
        else {
            _M_push_front(_x);
        }
    }
    void push_front(value_type&& _x) {
        emplace_front(std::move(_x));
    }
    void pop_front() {
        if (empty()) { return; }
        if (this->_data._start._cur != this->_data._start._last - 1) {
            _M_destroy_element(this->_data._start);
            ++this->_data._start;
        }
        else {
            _M_pop_front();
        }
    }
    template <typename... _Args> iterator emplace(const_iterator _pos, _Args&&... _args) {
        if (_pos._cur == this->_data._start._cur) {
            emplace_front(std::forward<_Args>(_args)...);
            return this->_data._start;
        }
        else if (_pos._cur == this->_data._finish._cur) {
            emplace_back(std::forward<_Args>(_args)...);
            iterator _tmp = this->_data._finish;
            --_tmp;
            return _tmp;
        }
        else {
            return _M_insert(_pos._M_const_cast(), std::forward<_Args>(_args)...);
        }
    }
    template <typename... _Args> void emplace_back(_Args&&... _args) {
        if (this->_data._finish._cur != this->_data._finish._last - 1) {
            this->_M_construct_node(this->_data._finish._cur, std::forward<_Args>(_args)...);
            ++this->_data._finish;
        }
        else {
            _M_push_back(std::forward<_Args>(_args)...);
        }
    }
    template <typename... _Args> void emplace_front(_Args&&... _args) {
        if (this->_data._start._cur != this->_data._start._first) {
            --this->_data._start;
            this->_M_construct_node(this->_data._start._cur, std::forward<_Args>(_args)...);
        }
        else {
            _M_push_front(std::forward<_Args>(_args)...);
        }
    }
    iterator insert(const_iterator _pos, const value_type& _e) {
        if (_pos._cur == this->_data._start._cur) {
            push_front(_e);
            return this->_data._start;
        }
        else if (_pos._cur == this->_data._finish._cur) {
            push_back(_e);
            iterator _tmp = this->_data._finish;
            --_tmp;
            return _tmp;
        }
        else {
            return _M_insert(_pos._M_const_cast(), _e);
        }
    }
    template <typename _InputIterator> iterator insert(const_iterator _pos, _InputIterator _first, _InputIterator _last) {
        const difference_type _offset = _pos - cbegin();
        _M_range_insert(_pos._M_const_cast(), _first, _last);
        return begin() + _offset;
    }
    iterator erase(const_iterator _pos) {
        if (_pos == cend()) return end();
        return _M_erase(_pos._M_const_cast());
    }
    iterator erase(const_iterator _first, const_iterator _last) {
        return _M_range_erase(_first._M_const_cast(), _last._M_const_cast());
    }
    void clear() {
        _M_destroy_element(begin(), end());
        this->_M_destroy_map(this->_data._start._node + 1, this->_data._finish._node + 1);
        this->_data._finish = this->_data._start;
    }


    /// ostream
    template <typename _T, typename _A> friend std::ostream& operator<<(std::ostream& os, const deque<_T, _A>& d) {
        os << '[';
        for (auto p = d.cbegin(); p != d.cend(); ++p) {
            os << p;
            if (p + 1 != d.cend()) {
                os << (p._M_next_block() ? "; " : ", ");
            }
        }
        os << ']';
        return os;
    }


protected:
    // to make sure at least %_nodes_to_add nodes exist at back, if there's no enough space, then reallocate.
    // promise to not change the distance between %_start and %_finish
    // need to allocate node
    void _M_reserve_map_at_back(size_type _nodes_to_add = 1) {
        if (_nodes_to_add + 1 > size_type(this->_data._map_size - (this->_data._finish._node - this->_data._map))) {
            _M_reallocate_map_at_back(_nodes_to_add);
        }
    }
    // to make sure at least %_nodes_to_add nodes exist at front, if there's no enough space, then reallocate.
    // promise to not change the distance between %_start and %_finish
    // need to allocate node
    void _M_reserve_map_at_front(size_type _nodes_to_add = 1) {
        if (_nodes_to_add > size_type(this->_data._start._node - this->_data._map)) {
            _M_reallocate_map_at_front(_nodes_to_add);
        }
    }
    // only reallocate memory, won't change the distance between %_start and %_finish
    void _M_reallocate_map_at_back(size_type _nodes_to_add) {
        const size_type _old_num_nodes = this->_data._finish._node - this->_data._start._node + 1;
        const size_type _new_num_nodes = _old_num_nodes + _nodes_to_add;
        map_pointer _new_nstart;
        // double the storage space, just like %vector
        if (this->_data._map_size > 2 * _new_num_nodes) {
            /*
            * @proof:
            * as known: %_map_size \gt  2*(%_new_num_nodes) = 2*(%_old_num_nodes + %_nodes_to_add)
            * noticed: %_map_size - (%_map_size - %_new_num_nodes) / 2 > %_nodes_to_add
            * so, there is enough storage space after {%_new_start + %_new_num_nodes}
            */
            _new_nstart = this->_data._map + (this->_data._map_size - _new_num_nodes) / 2;
            if (_new_nstart < this->_data._start._node) {
                std::copy(this->_data._start._node, this->_data._finish._node + 1, _new_nstart);
            }
            else {
                std::copy_backward(this->_data._start._node, this->_data._finish._node + 1, _new_nstart + _old_num_nodes);
            }
        }
        else {
            size_type _new_map_size = this->_data._map_size + (std::max(this->_data._map_size, _nodes_to_add)) + 2;
            map_pointer _new_map = this->_M_allocate_map(_new_map_size);
            _new_nstart = _new_map + (_new_map_size - _new_num_nodes) / 2;
            std::copy(this->_data._start._node, this->_data._finish._node + 1, _new_nstart);
            this->_M_deallocate_map(this->_data._map, this->_data._map_size);
            this->_data._map = _new_map;
            this->_data._map_size = _new_map_size;
        }
        this->_data._start._M_set_node(_new_nstart);
        this->_data._finish._M_set_node(_new_nstart + _old_num_nodes - 1);
    }
    // only reallocate memory, won't change the distance between %_start and %_finish
    void _M_reallocate_map_at_front(size_type _nodes_to_add) {
        const size_type _old_num_nodes = this->_data._finish._node - this->_data._start._node + 1;
        const size_type _new_num_nodes = _old_num_nodes + _nodes_to_add;
        map_pointer _new_nstart;
        if (this->_data._map_size > 2 * _new_num_nodes) {
            _new_nstart = this->_data._map + (this->_data._map_size - _new_num_nodes) / 2 + _nodes_to_add;
            if (_new_nstart < this->_data._start._node) {
                std::copy(this->_data._start._node, this->_data._finish._node + 1, _new_nstart);
            }
            else {
                std::copy_backward(this->_data._start._node, this->_data._finish._node + 1, _new_nstart + _old_num_nodes);
            }
        }
        else {
            size_type _new_map_size = this->_data._map_size + (std::max(this->_data._map_size, _nodes_to_add)) + 2;
            map_pointer _new_map = this->_M_allocate_map(_new_map_size);
            _new_nstart = _new_map + (_new_map_size - _new_num_nodes) / 2 + _nodes_to_add;
            std::copy(this->_data._start._node, this->_data._finish._node + 1, _new_nstart);
            this->_M_deallocate_map(this->_data._map, this->_data._map_size);
            this->_data._map = _new_map;
            this->_data._map_size = _new_map_size;
        }
        this->_data._start._M_set_node(_new_nstart);
        this->_data._finish._M_set_node(_new_nstart + _old_num_nodes - 1);
    }

    /* to make sure at least %_n elements exist at back, if there's no enough space, then reallocate.
    *  promise to not change the distance between %_start and %_finish */
    iterator _M_reserve_elements_at_back(size_type _n) {
        const size_type _vacancy = this->_data._start._last - this->_data._start._cur - 1;
        if (_n > _vacancy) {
            _M_new_elements_at_back(_n - _vacancy);
        }
        return this->_data._finish + difference_type(_n);
    }
    /* to make sure at least %_n elements exist at front, if there's no enough space, then reallocate.
    *  promise to not change the distance between %_start and %_finish */
    iterator _M_reserve_elements_at_front(size_type _n) {
        const size_type _vacancy = this->_data._start._cur - this->_data._start._first;
        if (_n > _vacancy) {
            _M_new_elements_at_front(_n - _vacancy);
        }
        return this->_data._start - difference_type(_n);
    }
    void _M_new_elements_at_back(size_type _new_elems) {
        const size_type _new_nodes = (_new_elems + base::_S_buffer_size() - 1) / base::_S_buffer_size();
        _M_reserve_map_at_back(_new_nodes);
        for (size_type _i = 1; _i <= _new_nodes; ++_i) {
            *(this->_data._finish._node + _i) = this->_M_allocate_node();
        }
    }
    void _M_new_elements_at_front(size_type _new_elems) {
        const size_type _new_nodes = (_new_elems + base::_S_buffer_size() - 1) / base::_S_buffer_size();
        _M_reserve_map_at_front(_new_nodes);
        for (size_type _i = 1; _i <= _new_nodes; ++_i) {
            *(this->_data._start._node - _i) = this->_M_allocate_node();
        }
    }

    void _M_destroy_element(iterator _pos) {
        this->_M_destroy_node(_pos._cur);
    }
    void _M_destroy_element(iterator _first, iterator _last) {
        for (map_pointer _np = _first._node + 1; _np < _last._node; ++_np) {
            this->_M_destroy_node(*_np, base::_S_buffer_size());
        }
        if (_first._node != _last._node) {
            this->_M_destroy_node(_first._cur, _first._last);
            this->_M_destroy_node(_last._first, _last._cur);
        }
        else {
            this->_M_destroy_node(_first._cur, _last._cur);
        }
    }

    /*
    * @brief: push the element to the back, which need to reallocate the memory.
    * @invoke: only when {%_data._finish._cur == %_data._finish._last - 1}
    */
    template <typename... _Args> void _M_push_back(_Args&&... _args) {
        _M_reserve_map_at_back();
        *(this->_data._finish._node + 1) = this->_M_allocate_node();
        this->_M_construct_node(this->_data._finish._cur, std::forward<_Args>(_args)...);
        this->_data._finish._M_set_node(this->_data._finish._node + 1);
        this->_data._finish._cur = this->_data._finish._first;
    }
    /*
    * @brief: pop the element on the back, which need to deallocate the memory.
    * @invoke: only when {%_data._finish._cur == %_data._finish._first}
    */
    void _M_pop_back() {
        this->_M_deallocate_node(this->_data._finish._first);
        this->_data._finish._M_set_node(this->_data._finish._node - 1);
        this->_data._finish._cur = this->_data._finish._last - 1;
        this->_M_destroy_node(this->_data._finish._cur);
    }
    /*
    * @brief: push the element to the front, which need to reallocate the memory.
    * @invoke: only when {%_data._start._cur == %_data._start._first}
    */
    template <typename... _Args> void _M_push_front(_Args&&... _args) {
        _M_reserve_map_at_front();
        *(this->_data._start._node - 1) = this->_M_allocate_node();
        this->_data._start._M_set_node(this->_data._start._node - 1);
        this->_data._start._cur = this->_data._start._last - 1;
        this->_M_construct_node(this->_data._start._cur, std::forward<_Args>(_args)...);
    }
    /*
    * @brief: pop the element on the front, which need to deallocate the memory.
    * @invoke: only when {%_data._start._cur == %_data._start._last - 1}
    */
    void _M_pop_front() {
        this->_M_destroy_node(this->_data._start._cur);
        this->_M_deallocate_node(this->_data._start._first);
        this->_data._start._M_set_node(this->_data._start._node + 1);
        this->_data._start._cur = this->_data._start._first;
    }

    template <typename... _Args> iterator _M_insert(iterator _pos, _Args&&... _args) {
        difference_type _index = _pos - this->_data._start;
        value_type _x_copy(std::forward<_Args>(_args)...);
        if (static_cast<size_type>(_index) < size() / 2) {
            push_front(std::move(front()));
            iterator _old_front = this->_data._start;
            ++_old_front;
            iterator _old_2rd_front = _old_front;
            ++_old_2rd_front;
            _pos = this->_data._start + _index;
            iterator _pos1 = _pos;
            ++_pos1;
            this->_M_range_copy(_old_2rd_front, _pos1, _old_front);
        }
        else {
            push_back(std::move(back()));
            iterator _old_finish = this->_data._finish;
            --_old_finish;
            iterator _old_2nd_finish = _old_finish;
            --_old_2nd_finish;
            _pos = this->_data._start + _index;
            this->_M_range_copy_backward(_pos, _old_2nd_finish, _old_finish);
        }
        *_pos = _x_copy;
        return _pos;
    }
    template <typename _ForwardIterator> void _M_insert_n(iterator _pos, _ForwardIterator _first, _ForwardIterator _last, size_type _n) {
        const difference_type _elems_before = _pos - this->_data._start;
        const size_type _old_length = size();
        if (static_cast<size_type>(_elems_before) < _old_length / 2) { // insert in the 1st half
            iterator _new_start = _M_reserve_elements_at_front(_n);
            iterator _old_start = this->_data._start;
            _pos = this->_data._start + _elems_before;
            if (_elems_before >= difference_type(_n)) {
                iterator _old_start_n = _old_start + _n;
                this->_M_range_copy(_old_start, _old_start_n, _new_start);
                this->_data._start = _new_start;
                this->_M_range_copy(_old_start_n, _pos, _old_start);
                asp::_A_copy(_first, _last, _pos - difference_type(_n), this->_data);
            }
            else {
                _ForwardIterator _mid = _first;
                asp::advance(_mid, difference_type(_n) - _elems_before);
                this->_M_range_copy(_old_start, _pos, _new_start);
                this->_data._start = _new_start;
                iterator _new_start_n = _new_start + _elems_before;
                asp::_A_copy(_first, _mid, _new_start_n, this->_data);
                asp::_A_copy(_mid, _last, _old_start, this->_data);
            }
        }
        else { // insert in the 2nd half
            iterator _new_finish = _M_reserve_elements_at_back(_n);
            iterator _old_finish = this->_data._finish;
            const difference_type _elems_after = difference_type(_old_length) - _elems_before;
            _pos = this->_data._finish - _elems_after;
            if (_elems_after > difference_type(_n)) {
                iterator _old_finish_n = _old_finish - difference_type(_n);
                this->_M_range_copy(_old_finish_n, _old_finish, _old_finish);
                this->_data._finish = _new_finish;
                this->_M_range_copy_backward(_pos, _old_finish_n, _old_finish);
                asp::_A_copy(_first, _last, _pos, this->_data);
            }
            else {
                _ForwardIterator _mid = _first;
                asp::advance(_mid, _elems_after);
                iterator _new_finish_n = _new_finish - _elems_after;
                this->_M_range_copy(_pos, _old_finish, _new_finish_n);
                this->_data._finish = _new_finish;
                asp::_A_copy(_first, _mid, _pos, this->_data);
                asp::_A_copy(_mid, _last, _old_finish, this->_data);
            }
        }
    }
    template <typename _ForwardIterator> void _M_range_insert(iterator _pos, _ForwardIterator _first, _ForwardIterator _last) {
        const size_type _n = asp::distance(_first, _last);
        if (_pos == this->_data._start) {
            iterator _new_start = _M_reserve_elements_at_front(_n);
            asp::_A_copy(_first, _last, _new_start, this->_data);
            this->_data._start = _new_start;
        }
        else if (_pos == this->_data._finish) {
            iterator _new_finish = _M_reserve_elements_at_back(_n);
            asp::_A_copy(_first, _last, this->_data._finish, this->_data);
            this->_data._finish = _new_finish;
        }
        else {
            this->_M_insert_n(_pos, _first, _last, _n);
        }
    }

    iterator _M_erase(iterator _pos) {
        iterator _next = _pos;
        ++_next;
        const difference_type _index = _pos - begin();
        if (static_cast<size_type>(_index) < (size() / 2)) {
            if (_pos != begin()) {
                this->_M_range_copy_backward(begin(), _pos, _next);
            }
            pop_front();
        }
        else {
            if (_pos != end()) {
                this->_M_range_copy(_next, end(), _pos);
            }
            pop_back();
        }
        return begin() + _index;
    }
    iterator _M_range_erase(iterator _first, iterator _last) {
        if (_first == _last) {
            return _first;
        }
        if (_first == begin() && _last == end()) {
            clear();
            return end();
        }
        const difference_type _n = _last - _first;
        const difference_type _elems_before = _first - begin();
        if (static_cast<size_type>(_elems_before) <= (size() - _n) / 2) {
            this->_M_range_copy_backward(begin(), _first, _last);
            iterator _pos = begin() + _n;
            _M_destroy_element(begin(), _pos);
            this->_M_destroy_map(this->_data._start._node, _pos._node);
            this->_data._start = _pos;
        }
        else {
            this->_M_range_copy(_last, end(), _first);
            iterator _pos = end() - _n;
            _M_destroy_element(_pos, end());
            this->_M_destroy_map(_pos._node + 1, this->_data._finish._node + 1);
            this->_data._finish = _pos;
        }
        return begin() + _elems_before;
    }
};

};

/*
map memory model:
    [%_map] [%_start] ... [%_finish] [] (%_map + %_map_size)
    each one is a node, which contains %data_type::_S_buffer_size() elements

iterator & node memory model:
    node => [%_first] [%_cur] ... [] (%_last)
    %iterator::_node => node
*/

#endif