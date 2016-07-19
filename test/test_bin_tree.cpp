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
    auto bt2 = bt;
    for (auto i = bt.rbegin(), iend = bt.rend(); i != iend; ++i)
        cout << *i << endl;
    return 0;
}
