#ifndef MINIDB_INDEX_MANAGER_H
#define MINIDB_INDEX_MANAGER_H

#include "hash_index.h"
#include "avl_index.h"
#include "schema.h"
#include "table_interface.h"
#include "schema.h"
#include <string>
#include <stdexcept>
#include <cstddef>
#include <vector>
#include <variant>
#include <cstdint>
namespace minidb {
    using namespace std ;
    class IndexManager {
    private:
     const Schema& schema; ;
     HashIndex hashIndex ; 
     AVLIndex avlIndex ;
      string indexedColumn ;
      size_t columnIndex ; 
      vector<Value> createEmptyValues() const {
            vector<Value> values;
            for (size_t i = 0; i < schema.columnCount(); i++) {
                switch (schema.getColumn(i).type) {
                    case DataType::INT:
                        values.push_back(std::int64_t(0));
                        break;
                    case DataType::FLOAT:
                        values.push_back(0.0);
                        break;
                    case DataType::STRING:
                        values.push_back(std::string(""));
                        break;
                }
            }
            return values;
        }
    
    public:
         IndexManager(const Schema& schema,const string& columnName): schema(schema),indexedColumn(columnName),columnIndex(schema.columnCount()){
            bool found = false ;
            for(size_t i=0 ; i<schema.columnCount();i++){
                if(schema.getColumn(i).name==columnName){
                    if(schema.getColumn(i).type!=DataType::INT){
                        throw invalid_argument("Indexing is supported only for INT columns");
                    }
                    columnIndex=i ; 
                    found = true ; 
                    break ; 
                }
            }
            if(!found){
                throw invalid_argument("Column not found");
            }
        }
        void insertRecord(TableInterface& table ,const Record& record){
            Record oldRecord(record.getId(),schema,createEmptyValues()) ;
            if(table.getRecord(record.getId(),oldRecord)){
                int oldKey = static_cast<int>(std::get<std::int64_t>(oldRecord.getValue(columnIndex))) ;
                hashIndex.erase(oldKey,record.getId()) ;
                avlIndex.erase(oldKey,record.getId()) ;

            }
            table.insertRecord(record) ;
            int key = static_cast<int>(std::get<std::int64_t>(record.getValue(columnIndex))) ;
            hashIndex.insert(key,record.getId()) ;
            avlIndex.insert(key,record.getId()) ;
        }
        bool deleteRecord(TableInterface& table ,RecordId id ){
            Record record(id,schema,createEmptyValues()) ;
            if(!table.getRecord(id,record)){
                return false ; 
            }
            int key = static_cast<int>(std::get<std::int64_t>(record.getValue(columnIndex))) ;
            hashIndex.erase(key,id) ;
            avlIndex.erase(key,id) ;
            return table.deleteRecord(id) ;
        }
        const HashIndex* getHashIndex() const {
            return &hashIndex;
        }
        const AVLIndex* getAVLIndex() const {
            return &avlIndex;
        }
        const string& getIndexedColumn() const {
            return indexedColumn;
        }
         };
        }

#endif