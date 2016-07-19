#ifndef BIN_TREE_H
#define BIN_TREE_H

#include <lc_config.h>
#include <construct.h>
#include <cstddef>

__LC_NAMESPACE_BEGIN

struct _BinaryTreeNodeBase {
    typedef _BinaryTreeNodeBase *_Base_ptr;
    _Base_ptr _M_left;
    _Base_ptr _M_right;
    _Base_ptr _M_parent;
    _Base_ptr leftmost(_Base_ptr p) {
        while (p)
            p = p->_M_left;
        return p;
    }
    _Base_ptr rightmost(_Base_ptr p) {
        while (p)
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
    BinaryTree(const BinaryTree&);
    ~BinaryTree() { clear(); _M_put_node(_M_header); }

    BinaryTree& operator=(const BinaryTree&);
    bool operator==(const BinaryTree&) const;
    bool operator!=(const BinaryTree&) const;
    bool operator<(const BinaryTree&) const; //optional
    bool operator>(const BinaryTree&) const; //optional
    bool operator<=(const BinaryTree&) const; //optional
    bool operator>=(const BinaryTree&) const; //optional

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
        return reverse_iterator(end());
    }
    reverse_iterator rend() { //optional
        return reverse_iterator(begin());
    }
    const_reverse_iterator rend() const { //optional
        return reverse_iterator(begin());
    }
    const_reverse_iterator crend() const { //optional
        return reverse_iterator(cbegin());
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
                x = (_Link_type)x->_M_right;
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
            construct(&tmp->_M_value_field, x);
        }
        __LC_UNWIND(_M_put_node(tmp));
        return tmp;
    }
    _Link_type _M_clone_node(_Link_type x) {
        _Link_type tmp = _M_create_node(x->_M_value_field);
        tmp->_M_left = nullptr;
        tmp->_M_right = nullptr;
        tmp->_M_is_header = false;
        return tmp;
    }
    void _M_destroy_node(_Link_type p) {
        destroy(&p->_M_value_field);
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

    // iterator& operator=(const iterator&);
    friend bool operator==(const bt_iterator &x, const bt_iterator &y) {
        return x._M_node == y._M_node;
    }
    friend bool operator!=(const bt_iterator &x, const bt_iterator &y) {
        return x._M_node != y._M_node;
    }

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
            if (_M_node->_M_right != y)
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
            if (_M_node->_M_left != y)
                _M_node = y;
        }
    }

    _Link_type _M_node;
};

template <class T, class A = std::allocator<_BinaryTreeNode<T>> >
void swap(BinaryTree<T, A> &lhs, BinaryTree<T, A> &rhs) { //optional
    lhs.swap(rhs);
    //std::swap(lhs._M_header, rhs._M_header);
    //std::swap(lhs._M_node_count, rhs._M_node_count);
}

__LC_NAMESPACE_END
#endif
