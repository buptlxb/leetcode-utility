#ifndef BIN_TREE_H
#define BIN_TREE_H

#include <lc_config.h>
#include <construct.h>
#include <cstddef>
#include <algorithm>

__LC_NAMESPACE_BEGIN

struct _BinaryTreeNodeBase {
    typedef _BinaryTreeNodeBase *_Base_ptr;
    _Base_ptr _M_left;
    _Base_ptr _M_right;
    _Base_ptr _M_parent;
    static _Base_ptr _S_leftmost(_Base_ptr p) {
        while (p->_M_left)
            p = p->_M_left;
        return p;
    }
    static _Base_ptr _S_rightmost(_Base_ptr p) {
        while (p->_M_right)
            p = p->_M_right;
        return p;
    }
};

template <typename Value>
struct _BinaryTreeNode : public _BinaryTreeNodeBase {
    typedef _BinaryTreeNode<Value> *_Link_type;
    Value _M_value_field;
    bool _M_is_header;
};

template <typename Value, typename Ref, typename Ptr>
class bt_iterator;

template <typename T, typename A = std::allocator<_BinaryTreeNode<T>> >
class BinaryTree {
public:
    typedef A allocator_type;
    typedef T value_type;
    typedef T& reference;
    typedef const T& const_reference;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef ptrdiff_t difference_type;
    typedef size_t size_type;
    typedef typename _BinaryTreeNode<T>::_Link_type _Link_type;
    typedef _BinaryTreeNodeBase *_Base_ptr;


    typedef bt_iterator<value_type, reference, pointer> iterator;
    typedef bt_iterator<value_type, const_reference, const_pointer> const_iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator; //optional
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator; //optional

public:
    BinaryTree() : _M_header(_M_get_node()), _M_node_count(0), _M_alloc() { _M_empty_initialize(); }
    BinaryTree(const BinaryTree &x) : _M_header(_M_get_node()), _M_node_count(0), _M_alloc() {
        if (x._M_root()) {
            _M_root() = _M_copy(x._M_root(), _M_header);
            _M_rightmost() = _S_rightmost(_M_root());
            _M_leftmost() = _S_leftmost(_M_root());
            _M_node_count = x._M_node_count;
        } else
            _M_empty_initialize();
    }
    ~BinaryTree() { clear(); _M_put_node(_M_header); }

    BinaryTree& operator=(const BinaryTree &x) {
        if (this != &x) {
            clear();
            _M_node_count = 0;
            if (x._M_root() == nullptr) {
                _M_root() = 0;
                _M_leftmost() = _M_header;
                _M_rightmost() = _M_header;
            } else {
                _M_root() = _M_copy(x._M_root(), _M_header);
                _M_leftmost() = (_Link_type)_S_leftmost(_M_root());
                _M_rightmost() = (_Link_type)_S_rightmost(_M_root());
                _M_node_count = x._M_node_count;
            }
        }
        return *this;
    }

    iterator begin() { return _M_leftmost(); }
    const_iterator begin() const { return _M_leftmost(); }
    const_iterator cbegin() const { return _M_leftmost(); }
    iterator end() { return _M_header; }
    const_iterator end() const { return _M_header; }
    const_iterator cend() const { return _M_header; }
    reverse_iterator rbegin() { //optional
        return reverse_iterator(end());
    }
    const_reverse_iterator rbegin() const { //optional
        return reverse_iterator(end());
    }
    const_reverse_iterator crbegin() const { //optional
        return const_reverse_iterator(cend());
    }
    reverse_iterator rend() { //optional
        return reverse_iterator(begin());
    }
    const_reverse_iterator rend() const { //optional
        return reverse_iterator(begin());
    }
    const_reverse_iterator crend() const { //optional
        return const_reverse_iterator(cbegin());
    }

    template<class ...Args>
    iterator emplace(const_iterator, Args...); //optional
    iterator insert(iterator pos, const T &v) {//optional
        _Link_type x = (_Link_type)pos._M_node;
        _Link_type n = _M_create_node(v);
        _S_left(n) = nullptr;
        _S_right(n) = nullptr;
        n->_M_is_header = false;
        if (_M_root()) {
            if (x == _M_leftmost()) {
                _S_left(x) = n;
                _M_leftmost() = n;
            } else if (x->_M_is_header) {
                x = _S_right(x);
                _S_right(x) = n;
                _M_rightmost() = n;
            } else {
                while (_S_left(x))
                    x = _S_left(x);
                _S_left(x) = n;
            }
            _S_parent(n) = x;
        } else {
            _M_root() = n;
            _M_leftmost() = n;
            _M_rightmost() = n;
            _S_parent(n) = _M_header;
        }
        ++_M_node_count;
        return iterator(n);
    }
    // iterator insert(const_iterator, T&&); //optional
    // iterator insert(const_iterator, size_type, T&); //optional
    // template<class iter>
    // iterator insert(const_iterator, iter, iter); //optional
    // iterator insert(const_iterator, std::initializer_list<T>); //optional
    // iterator erase(const_iterator); //optional
    // iterator erase(const_iterator, const_iterator); //optional
    void clear() { //optional
        if (_M_node_count != 0) {
            _M_erase(_M_root());
            _M_leftmost() = _M_header;
            _M_root() = 0;
            _M_rightmost() = _M_header;
            _M_node_count = 0;
        }
    }

    void swap(BinaryTree &t) {
        std::swap(_M_header, t._M_header);
        std::swap(_M_node_count, t._M_node_count);
    }

    size_type size() { return _M_node_count; }
    size_type max_size() { return size_type(-1); }
    bool empty() const { return _M_node_count == 0; }
    allocator_type get_allocator() const { return _M_alloc; } //optional

protected:
    _Link_type _M_get_node() { return _M_alloc.allocate(1); }
    void _M_put_node(_Link_type p) { _M_alloc.deallocate(p, 1); }

    _Link_type _M_create_node(const value_type& x) {
        _Link_type tmp = _M_get_node();
        __LC_TRY {
            construct(&_S_value(tmp), x);
        }
        __LC_UNWIND(_M_put_node(tmp));
        return tmp;
    }
    _Link_type _M_clone_node(_Link_type x) {
        _Link_type tmp = _M_create_node(_S_value(x));
        _S_left(tmp) = nullptr;
        _S_right(tmp) = nullptr;
        tmp->_M_is_header = x->_M_is_header;
        return tmp;
    }
    void _M_destroy_node(_Link_type p) {
        destroy(&_S_value(p));
        _M_put_node(p);
    }

    void _M_erase(_Link_type x) {
        while (x) {
            _M_erase(_S_right(x));
            _Link_type y = _S_left(x);
            _M_destroy_node(x);
            x = y;
        }
    }
    _Link_type _M_copy(_Link_type x, _Link_type p) {
        _Link_type top = _M_clone_node(x);
        _S_parent(top) = p;
        __LC_TRY {
            if (_S_right(x))
                _S_right(top) = _M_copy(_S_right(x), top);
            p = top;
            x = _S_left(x);
            while (x) {
                _Link_type y = _M_clone_node(x);
                _S_left(p) = y;
                _S_parent(y) = p;
                if (_S_right(x))
                    _S_right(y) = _M_copy(_S_right(x), y);
                p = y;
                x = _S_left(x);
            }
        }
        __LC_UNWIND(_M_erase(top));
        return top;
    }

    _Link_type& _M_root() const {
        return (_Link_type&) _M_header->_M_parent;
    }
    _Link_type& _M_leftmost() const {
        return (_Link_type&) _M_header->_M_left;
    }
    _Link_type& _M_rightmost() const {
        return (_Link_type&) _M_header->_M_right;
    }

    static _Link_type& _S_left(_Link_type x) {
        return (_Link_type&)(x->_M_left);
    }
    static _Link_type& _S_right(_Link_type x) {
        return (_Link_type&)(x->_M_right);
    }
    static _Link_type& _S_parent(_Link_type x) {
        return (_Link_type&)(x->_M_parent);
    }
    static reference _S_value(_Link_type x) {
        return x->_M_value_field;
    }

    static _Link_type& _S_left(_Base_ptr x) {
        return (_Link_type&)(x->_M_left);
    }
    static _Link_type& _S_right(_Base_ptr x) {
        return (_Link_type&)(x->_M_right);
    }
    static _Link_type& _S_parent(_Base_ptr x) {
        return (_Link_type&)(x->_M_parent);
    }
    static reference _S_value(_Base_ptr x) {
        return ((_Link_type)x)->_M_value_field;
    }
    static _Link_type _S_leftmost(_Link_type x) {
        return (_Link_type)_BinaryTreeNodeBase::_S_leftmost(x);
    }
    static _Link_type _S_rightmost(_Link_type x) {
        return (_Link_type)_BinaryTreeNodeBase::_S_rightmost(x);
    }

private:
    void _M_empty_initialize() {
        _M_header->_M_is_header = true;
        _M_root() = 0;
        _M_leftmost() = _M_header;
        _M_rightmost() = _M_header;
    }

protected:
    _Link_type _M_header;
    size_type _M_node_count;
    allocator_type _M_alloc;
};

template <typename T, typename A = std::allocator<_BinaryTreeNode<T>> >
bool operator==(const BinaryTree<T, A> &x, const BinaryTree<T, A> &y) {
    return std::equal(x.begin(), x.end(), y.begin(), y.end());
}

template <typename T, typename A = std::allocator<_BinaryTreeNode<T>> >
bool operator!=(const BinaryTree<T, A> &x, const BinaryTree<T, A> &y) {
    return !(x == y);
}


template <typename Value, typename Ref, typename Ptr>
struct bt_iterator { 
    typedef Value value_type;
    typedef Ref reference;
    typedef ptrdiff_t difference_type;
    typedef Ptr pointer;
    typedef std::bidirectional_iterator_tag iterator_category; //or another tag
    typedef typename _BinaryTreeNode<Value>::_Link_type _Link_type;

    bt_iterator() : _M_node(nullptr) {}
    bt_iterator(_Link_type x) : _M_node(x) {}
    bt_iterator(const bt_iterator& it) { _M_node = it._M_node; }
    // ~iterator();

    bt_iterator& operator++() {
        _M_increment();
        return *this;
    }
    bt_iterator operator++(int) {  //optional
        bt_iterator tmp = *this;
        _M_increment();
        return tmp;
    }
    bt_iterator& operator--() { //optional
        _M_decrement();
        return *this;
    }
    bt_iterator operator--(int) { //optional
        bt_iterator tmp = *this;
        _M_decrement();
        return tmp;
    }

    reference operator*() const { return _M_node->_M_value_field; }
    pointer operator->() const { return &(operator*()); }

    void _M_increment() {
        if (_M_node->_M_right) {
            _M_node = (_Link_type)_M_node->_M_right;
            while (_M_node->_M_left)
                _M_node = (_Link_type)_M_node->_M_left;
        } else {
            _Link_type y = (_Link_type)_M_node->_M_parent;
            while (_M_node == (_Link_type)y->_M_right) {
                _M_node = y;
                y = (_Link_type)y->_M_parent;
            }
            if ((_Link_type)_M_node->_M_right != y)
                _M_node = y;
        }
    }
    void _M_decrement() {
        if (_M_node->_M_is_header)
            _M_node = (_Link_type)_M_node->_M_right;
        else if (_M_node->_M_left) {
            _M_node = (_Link_type)_M_node->_M_left;
            while (_M_node->_M_right)
                _M_node = (_Link_type)_M_node->_M_right;
        } else {
            _Link_type y = (_Link_type)_M_node->_M_parent;
            while (_M_node == (_Link_type)y->_M_left) {
                _M_node = y;
                y = (_Link_type)y->_M_parent;
            }
            if ((_Link_type)_M_node->_M_left != y)
                _M_node = y;
        }
    }

    _Link_type _M_node;
};

template <typename V, typename R, typename P>
bool operator==(const bt_iterator<V, R, P> &x, const bt_iterator<V, R, P> &y) {
    return x._M_node == y._M_node;
}
template <typename V, typename R, typename P>
bool operator!=(const bt_iterator<V, R, P> &x, const bt_iterator<V, R, P> &y) {
    return x._M_node != y._M_node;
}


template <class T, class A = std::allocator<_BinaryTreeNode<T>> >
void swap(BinaryTree<T, A> &lhs, BinaryTree<T, A> &rhs) { //optional
    lhs.swap(rhs);
}

template <typename Value, typename Ref, typename Ptr>
inline std::bidirectional_iterator_tag iterator_category(const bt_iterator<Value, Ref, Ptr> &) {
    return std::bidirectional_iterator_tag();
}

template <typename Value, typename Ref, typename Ptr>
inline typename bt_iterator<Value, Ref, Ptr>::difference_type* distance_type(const bt_iterator<Value, Ref, Ptr> &) {
    return static_cast<typename bt_iterator<Value, Ref, Ptr>::difference_type *>(0);
}

template <typename Value, typename Ref, typename Ptr>
inline Value * value_type(const bt_iterator<Value, Ref, Ptr> &) {
    return static_cast<Value *>(0);
}

__LC_NAMESPACE_END
#endif
