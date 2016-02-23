#include <iostream>
#include <vector>
#include <queue>
#include "print.h"

using namespace std;
using namespace leetcode;

int main(void)
{
    vector<int> vec{1, 2, 3, 4, 5};
    queue<int> q;
    for (auto x : vec)
        q.push(x);
    cout << vec << endl;
    cout << q << endl;
}
