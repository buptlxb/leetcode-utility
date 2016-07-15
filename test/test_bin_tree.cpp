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
    return 0;
}
