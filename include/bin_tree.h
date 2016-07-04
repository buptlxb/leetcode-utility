#ifndef BIN_TREE_H
#define BIN_TREE_H

#include <lc_config.h>
#include <construct.h>

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
};

template <typename T, typename A = std::allocator<_BinaryTreeNode<T>> >
class BinaryTree {
public:
    typedef A allocator_type;
    typedef typename A::value_type value_type; 
    typedef typename A::reference reference;
    typedef typename A::const_reference const_reference;
    typedef typename A::difference_type difference_type;
    typedef typename A::size_type size_type;
    typedef typename _BinaryTreeNode<T>::_Link_type _Link_type;
    typedef _BinaryTreeNodeBase *_Base_ptr;

    class iterator { 
    public:
        typedef typename A::difference_type difference_type;
        typedef typename A::value_type value_type;
        typedef typename A::reference reference;
        typedef typename A::pointer pointer;
        typedef std::bidirectional_iterator_tag iterator_category; //or another tag

        iterator();
        iterator(const iterator&);
        ~iterator();

        iterator& operator=(const iterator&);
        bool operator==(const iterator&) const;
        bool operator!=(const iterator&) const;
        bool operator<(const iterator&) const; //optional
        bool operator>(const iterator&) const; //optional
        bool operator<=(const iterator&) const; //optional
        bool operator>=(const iterator&) const; //optional

        iterator& operator++();
        iterator operator++(int); //optional
        iterator& operator--(); //optional
        iterator operator--(int); //optional

        reference operator*() const;
        pointer operator->() const;
    };
    class const_iterator {
    public:
        typedef typename A::difference_type difference_type;
        typedef typename A::value_type value_type;
        typedef typename A::reference const_reference;
        typedef typename A::pointer const_pointer;
        typedef std::bidirectional_iterator_tag iterator_category; //or another tag

        const_iterator ();
        const_iterator (const const_iterator&);
        const_iterator (const iterator&);
        ~const_iterator();

        const_iterator& operator=(const const_iterator&);
        bool operator==(const const_iterator&) const;
        bool operator!=(const const_iterator&) const;
        bool operator<(const const_iterator&) const; //optional
        bool operator>(const const_iterator&) const; //optional
        bool operator<=(const const_iterator&) const; //optional
        bool operator>=(const const_iterator&) const; //optional

        const_iterator& operator++();
        const_iterator operator++(int); //optional
        const_iterator& operator--(); //optional
        const_iterator operator--(int); //optional

        const_reference operator*() const;
        const_pointer operator->() const;
    };

    typedef std::reverse_iterator<iterator> reverse_iterator; //optional
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator; //optional

public:
    BinaryTree() : _M_header(_M_get_node()), _M_node_count(0), _M_alloc() { _M_empty_initialize(); }
    BinaryTree(const BinaryTree&);
    ~BinaryTree() { clear(); }

    BinaryTree& operator=(const BinaryTree&);
    bool operator==(const BinaryTree&) const;
    bool operator!=(const BinaryTree&) const;
    bool operator<(const BinaryTree&) const; //optional
    bool operator>(const BinaryTree&) const; //optional
    bool operator<=(const BinaryTree&) const; //optional
    bool operator>=(const BinaryTree&) const; //optional

    iterator begin();
    const_iterator begin() const;
    const_iterator cbegin() const;
    iterator end();
    const_iterator end() const;
    const_iterator cend() const;
    reverse_iterator rbegin(); //optional
    const_reverse_iterator rbegin() const; //optional
    const_reverse_iterator crbegin() const; //optional
    reverse_iterator rend(); //optional
    const_reverse_iterator rend() const; //optional
    const_reverse_iterator crend() const; //optional

    template<class ...Args>
    iterator emplace(const_iterator, Args...); //optional
    iterator insert(const_iterator, const T&); //optional
    iterator insert(const_iterator, T&&); //optional
    iterator insert(const_iterator, size_type, T&); //optional
    template<class iter>
    iterator insert(const_iterator, iter, iter); //optional
    iterator insert(const_iterator, std::initializer_list<T>); //optional
    iterator erase(const_iterator); //optional
    iterator erase(const_iterator, const_iterator); //optional
    void clear() { //optional
        if (_M_node_count != 0) {
            _M_erase(_M_root());
            _M_leftmost() = _M_header;
            _M_root() = 0;
            _M_rightmost() = _M_header;
            _M_node_count = 0;
        }
    }

    void swap(BinaryTree&);
    size_type size();
    size_type max_size();
    bool empty();
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
        _M_root() = 0;
        _M_leftmost() = _M_header;
        _M_rightmost() = _M_header;
    }

protected:
    _Link_type _M_header;
    size_type _M_node_count;
    allocator_type _M_alloc;
};
template <class T, class A = std::allocator<T> >
void swap(BinaryTree<T,A>&, BinaryTree<T,A>&); //optional

__LC_NAMESPACE_END
#endif
