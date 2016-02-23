#ifndef BINARY_TREE_HH
#define BINARY_TREE_HH

#include <iterator>
#include <memory>
#include <queue>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <stack>

namespace leetcode {

    class TreeNodeBase {
        public:
            typedef TreeNodeBase *base_ptr;
            base_ptr left, right;
            TreeNodeBase() : left(nullptr), right(nullptr) {}
    };

    template <typename ValueType>
    class TreeNode : public TreeNodeBase {
        public:
            typedef ValueType value_type;
            typedef TreeNode *link_type;
            ValueType val;
            TreeNode(int x) : val(x), TreeNodeBase() {}
    };

    template <typename ValueType>
    class Tree {
        public:
            typedef ValueType value_type;
            typedef TreeNode<value_type> node_type;
            typedef size_t size_type;
            typedef node_type *link_type;
            typedef std::allocator<node_type> tree_node_allocator;
        public:
            bool empty() { return root == nullptr; }
            size_type size() { return nodes; }

            Tree() : root(nullptr), nodes(0) {}

            template <typename InputIterator>
            Tree(InputIterator first, InputIterator last, ValueType nil) : root(nullptr), nodes(0)  {
                root = build_tree(first, last, nil);
            }

            ~Tree() { destory(root); }
            std::string serialize(std::string nil="null", std::string delim=", ");
            bool deserialize(std::string str, std::string nil="null", std::string delim=", ");
            void preorder(void (*)(link_type));
            void preorder(link_type root, void (*)(link_type));
            void preorderIter(void (*)(link_type));
            void preorderThreaded(void (*)(link_type));
            void inorder(void (*)(link_type));
            void inorder(link_type root, void (*)(link_type));
            void inorderIter(void (*)(link_type));
            void inorderThreaded(void (*)(link_type));
            void postorder(void (*)(link_type));
            void postorder(link_type root, void (*)(link_type));
            void postorderIter(void (*)(link_type));
            void postorderThreaded(void (*)(link_type));
            link_type leftmost(link_type root) {
                if (!root)
                    return nullptr;
                while (root->left)
                    root = static_cast<link_type>(root->left);
                return root;
            }
            link_type leftmost() { return leftmost(root); }
            link_type rightmost(link_type root) {
                if (!root)
                    return nullptr;
                while (root->right)
                    root = static_cast<link_type>(root->right);
                return root;
            }
            link_type rightmost() { return rightmost(root); }
        private:
            link_type root;
            size_type nodes;
            tree_node_allocator alloc;
        private:
            void destory(link_type root) {
                if (!root)
                    return;
                destory(static_cast<link_type>(root->left));
                destory(static_cast<link_type>(root->right));
                put_node(root);
                root = nullptr;
            }
            void put_node(link_type p) { alloc.deallocate(p, 1); }
            link_type get_node() { return alloc.allocate(1); }
            link_type create_node(const value_type &x) {
                link_type tmp = get_node();
                try {
                    alloc.construct(&tmp->val, x);
                } catch (...) {
                    std::cerr << "create_node failed" << std::endl;
                    put_node(tmp);
                }
                tmp->left = nullptr;
                tmp->right = nullptr;
                return tmp;
            }
            template <typename InputIterator>
            link_type build_tree(InputIterator, InputIterator, ValueType nil);
            void reverse(link_type first, link_type last) {
                while (first != last) {
                    link_type cur = first;
                    first = static_cast<link_type>(first->right);
                    cur->right = last->right;
                    last->right = cur;
                }
            }
    };
    template <typename ValueType>
    template <typename InputIterator>
    typename Tree<ValueType>::link_type Tree<ValueType>::build_tree(InputIterator first, InputIterator last, ValueType nil) {
        if (first == last || !*first)
            return nullptr;
        std::queue<link_type> q;
        root = create_node(*first++);
        q.push(root);
        ++nodes;
        while (!q.empty() && first != last) {
            link_type cur = q.front();
            q.pop();
            if (*first != nil) {
                ++nodes;
                cur->left = create_node(*first);
                q.push(static_cast<link_type>(cur->left));
            }
            ++first;
            if (first == last)
                break;
            if (*first != nil) {
                ++nodes;
                cur->right = create_node(*first);
                q.push(static_cast<link_type>(cur->right));
            }
            ++first;
        }
        return root;
    }
    template <typename ValueType>
    std::string Tree<ValueType>::serialize(std::string nil, std::string delim) {
        std::ostringstream os;
        std::queue<link_type> q;
        q.push(root);
        size_type cnt = 0;
        os << "{";
        while (cnt != nodes) {
            link_type cur = q.front();
            q.pop();
            if (cur != nullptr) {
                q.push(static_cast<link_type>(cur->left));
                q.push(static_cast<link_type>(cur->right));
                os << cur->val;
                if (++cnt != nodes)
                    os << delim;
            } else {
                os << nil << delim;
            }
        }
        os << "}";
        return os.str();
    }
    template <typename ValueType>
    bool Tree<ValueType>::deserialize(std::string str, std::string nil, std::string delim) {
        if (str.size() < 2 || *str.begin() != '{' || *str.rbegin() != '}')
            return false;
        std::queue<link_type> q;
        bool left = true;
        bool ret = true;
        for (std::string::iterator i = str.begin()+1, start = i, iend = str.end(); i != iend; ++i) {
            if (!equal(delim.begin(), delim.end(), i) && i+1 != iend)
                continue;
            std::string token(start, i);
            if (token == nil) {
                if (q.empty()) {
                    ret = false;
                    break;
                }
                if (!left)
                    q.pop();
                left = !left;
            } else {
                ++nodes;
                std::istringstream is(token);
                ValueType tmp;
                is >> tmp;
                link_type cur = create_node(tmp);
                q.push(cur);
                if (root == nullptr) {
                    root = cur;
                } else {
                    if (q.empty()) {
                        ret = false;
                        break;
                    }
                    if (!left) {
                        q.front()->right = cur;
                        q.pop();
                    } else {
                        q.front()->left = cur;
                    }
                    left = !left;
                }
            }
            start = i+1 == iend ? i+1 : i + delim.size();
        }
        if (!ret) {
            destory(root);
            nodes = 0;
            root = nullptr;
        }
        return ret;
    }
    template <typename ValueType>
    void Tree<ValueType>::preorder(void (*access)(link_type)) {
        preorder(root, access);
    }
    template <typename ValueType>
    void Tree<ValueType>::preorder(link_type root, void (*access)(link_type)) {
        if (!root)
            return;
        access(root);
        preorder(static_cast<link_type>(root->left), access);
        preorder(static_cast<link_type>(root->right), access);
    }
    template <typename ValueType>
    void Tree<ValueType>::preorderIter(void (*access)(link_type)) {
        link_type cur = root;
        std::stack<link_type> s;
        while (!s.empty() || cur) {
            if (cur) {
                access(cur);
                s.push(cur);
                cur = static_cast<link_type>(cur->left);
            } else {
                cur = s.top();
                s.pop();
                cur = static_cast<link_type>(cur->right);
            }
        }
    }
    template <typename ValueType>
    void Tree<ValueType>::preorderThreaded(void (*access)(link_type)) {
        link_type cur = root;
        while (cur) {
            if (cur->left) {
                link_type rm = static_cast<link_type>(cur->left);
                while (rm->right && rm->right != cur)
                    rm = static_cast<link_type>(cur->right);
                if (rm->right == cur) {
                    cur = static_cast<link_type>(cur->right);
                    rm->right = nullptr;
                } else {
                    access(cur);
                    rm->right = cur;
                    cur = static_cast<link_type>(cur->left);
                }
            } else {
                access(cur);
                cur = static_cast<link_type>(cur->right);
            }
        }
    }
    template <typename ValueType>
    void Tree<ValueType>::inorder(void (*access)(link_type)) {
        inorder(root, access);
    }
    template <typename ValueType>
    void Tree<ValueType>::inorder(link_type root, void (*access)(link_type)) {
        if (!root)
            return;
        inorder(static_cast<link_type>(root->left), access);
        access(root);
        inorder(static_cast<link_type>(root->right), access);
    }
    template <typename ValueType>
    void Tree<ValueType>::inorderIter(void (*access)(link_type)) {
        link_type cur = root;
        std::stack<link_type> s;
        while (!s.empty() || cur) {
            if (cur) {
                s.push(cur);
                cur = static_cast<link_type>(cur->left);
            } else {
                cur = s.top();
                access(cur);
                s.pop();
                cur = static_cast<link_type>(cur->right);
            }
        }
    }
    template <typename ValueType>
    void Tree<ValueType>::inorderThreaded(void (*access)(link_type)) {
        link_type cur = root;
        while (cur) {
            if (cur->left) {
                link_type rm = static_cast<link_type>(cur->left);
                while (rm->right && rm->right != cur)
                    rm = static_cast<link_type>(rm->right);
                if (rm->right == cur) {
                    access(cur);
                    cur = static_cast<link_type>(cur->right);
                    rm->right = nullptr;
                } else {
                    rm->right = cur;
                    cur = static_cast<link_type>(cur->left);
                }
            } else {
                access(cur);
                cur = static_cast<link_type>(cur->right);
            }
        }
    }
    template <typename ValueType>
    void Tree<ValueType>::postorder(void (*access)(link_type)) {
        postorder(root, access);
    }
    template <typename ValueType>
    void Tree<ValueType>::postorder(link_type root, void (*access)(link_type)) {
        if (!root)
            return;
        postorder(static_cast<link_type>(root->left), access);
        postorder(static_cast<link_type>(root->right), access);
        access(root);
    }
    template <typename ValueType>
    void Tree<ValueType>::postorderIter(void (*access)(link_type)) {
        link_type cur = root;
        std::stack<link_type> s;
        link_type prev = nullptr;
        while (!s.empty() || cur) {
            if (cur) {
                s.push(cur);
                cur = static_cast<link_type>(cur->left);
            } else {
                cur = s.top();
                if (cur->right == nullptr || cur->right == prev) {
                    s.pop();
                    access(cur);
                    prev = cur;
                    cur = nullptr;
                } else
                    cur = static_cast<link_type>(cur->right);
            }
        }
    }
    template <typename ValueType>
    void Tree<ValueType>::postorderThreaded(void (*access)(link_type)) {
        TreeNode<value_type> dummy(-1);
        dummy.left = root;
        link_type cur = &dummy;
        link_type prev = nullptr;
        while (cur) {
            if (cur->left) {
                link_type rm = static_cast<link_type>(cur->left);
                while (rm->right && rm->right != cur)
                    rm = static_cast<link_type>(rm->right);
                if (rm->right == cur) {
                    rm->right = nullptr;
                    reverse(static_cast<link_type>(cur->left), rm);
                    for (link_type i = rm; i; i = static_cast<link_type>(i->right))
                        access(i);
                    reverse(rm, static_cast<link_type>(cur->left));
                    cur = static_cast<link_type>(cur->right);
                } else {
                    rm->right = cur;
                    cur = static_cast<link_type>(cur->left);
                }
            } else {
                cur = static_cast<link_type>(cur->right);
            }
        }
    }
};
#endif
