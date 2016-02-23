#ifndef PRINT_H
#define PRINT_H

#include <iostream>
#include <vector>
#include <queue>

namespace leetcode {
    template <typename InputIterator>
    std::ostream & print(std::ostream &os, InputIterator first, InputIterator last, std::string delim=", ") {
        if (first == last)
            return os;
        os << *first;
        while (++first != last)
            os << delim << *first;
        return os;
    }

    template <typename T>
    std::ostream & operator<< (std::ostream &os, const std::vector<T> &vec) {
        return print(os, vec.begin(), vec.end(), ", ");
    }

    template <typename T>
    std::ostream & operator<< (std::ostream &os, const std::queue<T> &q) {
        std::queue<T> t(q);
        while (t.size() > 1) {
            os << t.front() << ", ";
            t.pop();
        }
        if (!t.empty())
            os << t.front();
        return os;
    }
};
#endif
