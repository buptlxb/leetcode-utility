#include "bin_tree.h"
#include "tester.h"
#include <iostream>

using namespace std;

int main(void)
{
    lc::BinaryTree<lc::Tester> bt;
    lc::BinaryTree<lc::Tester>::iterator iter = bt.begin();
    for (int i = 0; i < 10; ++i)
        iter = bt.insert(iter, lc::Tester());
    {
        lc::BinaryTree<lc::Tester> tmp;
        tmp.swap(bt);
        for (auto i = tmp.begin(), iend = tmp.end(); i != iend; ++i)
            cout << *i << endl;
        for (auto &b : tmp)
            cout << b << endl;
    }
    return 0;
}
