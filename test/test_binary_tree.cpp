#include "binary_tree.h"
#include <vector>
#include <iostream>

template <typename T>
void access(typename leetcode::Tree<T>::link_type node) {
    std::cout << std::hex << node << ":" << std::dec << "(" << node->val << "," << std::hex << node->left << ", " << node->right << std::dec << ")" << std::endl;
}

int main(void)
{
    std::vector<int> vec{5,4,7,3,0,2,0,-1,0,9};
    leetcode::Tree<int> tree(vec.begin(), vec.end(), 0);
    std::cout << "size:" << tree.size() << std::endl;

    std::cout << "preorder:" << std::endl;
    tree.preorder(access<int>);
    std::cout << "preorderIter:" << std::endl;
    tree.preorderIter(access<int>);
    std::cout << "preorderThreaded:" << std::endl;
    tree.preorderThreaded(access<int>);

    std::cout << "inorder:" << std::endl;
    tree.inorder(access<int>);
    std::cout << "inorderIter:" << std::endl;
    tree.inorderIter(access<int>);
    std::cout << "inorderThreaded:" << std::endl;
    tree.inorderThreaded(access<int>);

    std::cout << "postorder:" << std::endl;
    tree.postorder(access<int>);
    std::cout << "postorderIter:" << std::endl;
    tree.postorderIter(access<int>);
    std::cout << "postorderThreaded:" << std::endl;
    tree.postorderThreaded(access<int>);

    std::cout << tree.serialize() << std::endl;
    leetcode::Tree<int> alt;
    if (alt.deserialize(tree.serialize()))
        std::cout << alt.serialize() << std::endl;
    else
        std::cout << "deserialize failed" << std::endl;
    return 0;
}
