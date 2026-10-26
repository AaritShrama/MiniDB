#include "avl_index.h"
#include <iostream>
#include <cassert>
#include <vector>

using namespace std;

void testRecordDeletion() {
    AVLIndex index;

    index.insert(20, 103);
    index.insert(20, 104);
    index.insert(10, 101);

    assert(index.erase(20, 103));

    auto remaining = index.find(20);
    assert(remaining.size() == 1);
    assert(remaining[0] == 104);

    assert(!index.erase(20, 999));

    assert(index.erase(20, 104));
    assert(index.find(20).empty());

    auto keys = index.getSortedKeys();
    assert((keys == vector<int>{10}));

    cout << "AVL record deletion test passed!\n";
}


int main() {
//     AVLIndex index;

//     index.insert(30, 101);
//     index.insert(10, 102);
//     index.insert(20, 103);
//     index.insert(20, 104);

// index.eraseKey(20);

// assert(index.find(20).empty());

// vector<int> remaining = index.getSortedKeys();
// vector<int> expectedRemaining = {10, 30};

// assert(remaining == expectedRemaining);

// cout << "AVL key deletion test passed!\n";
testRecordDeletion() ;
    return 0;
}