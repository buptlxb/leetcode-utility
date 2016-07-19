#ifndef TESTER_H
#define TESTER_H

#include <cassert>
#include <lc_config.h>
#include <iostream>

__LC_NAMESPACE_BEGIN

struct Verify;
class Tester {
    friend Verify;
    static int livecount;
    const Tester* self;
public:
    Tester() :self(this) {++livecount;}
    Tester(const Tester&) :self(this) {++livecount;}
    ~Tester() {assert(self==this);--livecount;}
    Tester& operator=(const Tester& b) {
        assert(self==this && b.self == &b);
        return *this;
    }
    void cfunction() const {assert(self==this);}
    void mfunction() {assert(self==this);}
    friend std::ostream &operator<< (std::ostream &os, const Tester &t) {
        os << "tester@" << std::hex << t.self << std::dec;
        return os;
    }
};
int Tester::livecount=0;
struct Verify {
    ~Verify() { assert(Tester::livecount==0); }
}varifier;


__LC_NAMESPACE_END

#endif
