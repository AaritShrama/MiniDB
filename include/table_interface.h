#ifndef MINIDB_TABLE_INTERFACE_H
#define MINIDB_TABLE_INTERFACE_H
#include "record.h" 
#include <vector>
using namespace std ;  
class TableInterface{
    public:
     virtual ~TableInterface() =default ; 
     virtual bool getRecord(RecordID id , Record& result) const = 0 ; 
     virtual vector<RecordID> getALLRecordIds() const = 0 ; 
};
#endif