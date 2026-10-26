#include "hash_index.h" 
#include <cassert> 
#include <iostream>  
#include <vector> 
using namespace std ; 
int main(){
    HashIndex index(10) ; 
    index.insert(20,101) ; 
    index.insert(21,102) ;
     index.insert(20,103) ; 
     index.insert(30, 105);
index.insert(40, 106);
    vector<IndexRecordId> results = index.find(20) ; 
    cout << "Found IDs: ";
for (IndexRecordId id : results) {
    std::cout << id << " ";
}
std::cout << '\n';
    assert(results.size()==2) ; 
    bool found101 =false ; 
    bool found103= false ; 
    for(IndexRecordId id: results){
        if(id==101) found101=true ; 
        if(id==103) found103 = true ; 
    }
assert(found101 && found103); 
assert(index.find(99).empty()) ;
assert(index.find(30).size() == 1);
assert(index.find(30)[0] == 105);
assert(index.find(40).size() == 1);
assert(index.find(40)[0] == 106);
assert(index.size() == 5);
assert(index.erase(20, 101));
assert(index.find(20).size() == 1);
assert(index.find(20)[0] == 103);
// The same entry cannot be deleted twice.
assert(!index.erase(20, 101));
// Deleting a nonexistent key-record pair must fail.
assert(!index.erase(99, 999));
// deletion.
assert(index.size() == 4);
//  resizing.
for (int i = 0; i < 20; ++i) {
    index.insert(100 + i, 1000 + i);
}
// Verify entries remain searchable after resizing.
for (int i = 0; i < 20; ++i) {
    std::vector<IndexRecordId> found =
        index.find(100 + i);
    assert(found.size() == 1);
    assert(found[0] == 1000 + i);
}
assert(index.size() == 24);
cout << "Hash Index resizing tests passed!\n";
cout << "Hash Index deletion tests passed!\n";
    cout <<"Hash Index basic test passed\n" ; 
    return 0  ; 
}