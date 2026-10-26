#ifndef MINIDB_HASH_INDEX_H
#define MINIDB_HASH_INDEX_H 
#include<cstddef> 
#include<cstdint> 
#include <forward_list> 
#include <vector>
#include <utility>
using namespace std ;  
using IndexRecordId  = uint64_t ; 
class HashIndex{
    private:
    struct Entry{
        int key  ; 
        IndexRecordId recordId  ;
    } ;
    vector<forward_list<Entry>> buckets  ;
    size_t entryCount ; 
    size_t hashFunction(int key )const {
        return static_cast< size_t>(key) % buckets.size() ; 
    }
    void resize(){
    vector<forward_list<Entry>> newBuckets(buckets.size()*2);
    for(const auto& bucket : buckets){
        for(const  Entry& entry :bucket){
            size_t newBucket = static_cast<size_t>(entry.key) % newBuckets.size() ; 
            newBuckets[newBucket].push_front(entry) ;
        }
    }
    buckets = move(newBuckets) ; 
    }
    public :
    
    explicit HashIndex(size_t bucketCount = 10 )
        : buckets(bucketCount==0 ?1:bucketCount),
        entryCount(0) {}
    void insert(int key , IndexRecordId recordId){
        size_t bucket = hashFunction(key) ;
        buckets[bucket].push_front({key,recordId}) ;
        ++entryCount  ;
        double loadFactor = static_cast<double>(entryCount)/buckets.size() ; 
        if(loadFactor>0.75){
            resize() ; 
        }
    };
    bool erase(int key , IndexRecordId recordId){
        size_t bucket = hashFunction(key) ; 
        auto& entries  =buckets[bucket] ; 
        auto previous = entries.before_begin() ; 
    for(auto current = entries.begin() ;current !=entries.end() ; ++current){
        if(current->key ==key && current->recordId == recordId){
            entries.erase_after(previous) ; 
            --entryCount ; 
            return true ; 
        }
        ++previous ; 
    }
    return false ; 
    }
    
    vector<IndexRecordId> find(int key) const{
        vector<IndexRecordId> result ; 
        size_t bucket = hashFunction(key) ;  
        for(const Entry& entry:buckets[bucket]){
            if(entry.key==key){
                result.push_back(entry.recordId);
            }
        }
        return result ; 
    }
    size_t size() const{
        return entryCount ; 
    }
} ; 
#endif