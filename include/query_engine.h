#ifndef MINIDB_QUERY_ENGINE_H
#define MINIDB_QUERY_ENGINE_H  
#include "query_parser.h"
#include "table_interface.h"
#include "schema.h"
#include "hash_index.h"
#include "avl_index.h"
#include <vector>
#include <string>
#include <stdexcept>
#include <cstdint>
namespace minidb {
    using namespace std ;
class QueryEngine {
public:
    static std::vector<Record> execute(
        const std::string& sql,
        const TableInterface& table,
        const Schema& schema,
        const HashIndex* hashIndex = nullptr,
        const AVLIndex* avlIndex = nullptr,
        const string& hashIndexColumn="",
        const string& avlIndexColumn="" ) {
        ParsedQuery query = QueryParser::parse(sql);
        if (query.selectedColumns.size() != 1 ||
            query.selectedColumns[0] != "*") {
            throw std::invalid_argument(
                "Currently only SELECT * is supported"
            );
        }
        std::vector<Record> results;
        std::vector<RecordId> ids = table.getALLRecordIds();
        // SELECT * FROM students
        if (!query.hasCondition) {
            for (RecordId id : ids) {
                vector<Value> emptyValues; 
                for(std::size_t i= 0; i<schema.columnCount();i++){
                    switch(schema.getColumn(i).type){
                        case DataType::INT:
                            emptyValues.push_back(std::int64_t(0));
                            break;
                        case DataType::FLOAT:
                            emptyValues.push_back(0.0);
                            break;
                        case DataType::STRING:
                            emptyValues.push_back(std::string(""));
                            break;
                    }
                }
                Record record(id, schema, emptyValues);
                if (table.getRecord(id, record)) {
                    results.push_back(record);
                }
            }
            return results;
        }
        // Currently supporting equality conditions only.
        if (query.condition.op != "=" && query.condition.op != ">"&& query.condition.op != "<") {
            throw std::invalid_argument("not supported");
        }
        // Find the column index.
        std::size_t columnIndex = schema.columnCount();
        bool found = false;

        for (std::size_t i = 0; i < schema.columnCount(); i++) {
            if (schema.getColumn(i).name == query.condition.column) {
                columnIndex = i;
                found = true;
                break;
            }
        }
        if (!found) {
            throw std::invalid_argument(
                "Unknown column: " + query.condition.column
            );
        }
        // Convert the condition value to the correct data type.
        Value expected;
        switch (schema.getColumn(columnIndex).type) {
            case DataType::INT:
                expected = static_cast<std::int64_t>(
                    std::stoll(query.condition.value)
                );
                break;
            case DataType::FLOAT:
                expected = std::stod(query.condition.value);
                break;
            case DataType::STRING: {
                std::string val = query.condition.value;
                if (val.size() >= 2 && ((val.front() == '\'' && val.back() == '\'') ||
                                        (val.front() == '"' && val.back() == '"'))) {
                    val = val.substr(1, val.size() - 2);
                }
                expected = val;
                break;
            }
        }
        // Search records using a table scan.
        vector<RecordId> candidateIds ; 
        bool isIntegerColumn = schema.getColumn(columnIndex).type == DataType::INT;
        if (query.condition.op != "=" && !isIntegerColumn) {
    throw std::invalid_argument(
        "Range queries currently support only INT columns"
    );
}
        bool useHashIndex =hashIndex != nullptr && hashIndexColumn == query.condition.column && isIntegerColumn && query.condition.op == "=";
        bool useAVLIndex = avlIndex != nullptr && avlIndexColumn == query.condition.column && isIntegerColumn;
        if(useHashIndex){
            int searchKey = static_cast<int>(std::get<std::int64_t>(expected));
            candidateIds = hashIndex->find(searchKey) ; 
        }
        else if(useAVLIndex){
            int searchKey  = static_cast<int>(std::get<std::int64_t>(expected)) ;
            vector<int> sortedKeys = avlIndex->getSortedKeys() ;
            for(int key:sortedKeys){
                bool matches = false ;
                if(query.condition.op=="="){
                    matches = (key==searchKey) ; 
                }
                else if(query.condition.op==">"){
                    matches = (key>searchKey) ; 
                }
                else if(query.condition.op=="<"){
                    matches = (key<searchKey) ; 
                }
                if(matches){
                    vector<AVLRecordId> recordIds = avlIndex->find(key) ;
                    for(AVLRecordId id:recordIds){
                        candidateIds.push_back(id) ; 
                    }
                }
            }
        }
        else{
            candidateIds = ids ;
        }
        for(RecordId id:candidateIds){
            vector<Value> emptyValues ; 
            for(size_t i= 0 ; i<schema.columnCount();i++){
                switch(schema.getColumn(i).type){
                    case DataType::INT:
                        emptyValues.push_back(std::int64_t(0));
                        break;
                    case DataType::FLOAT:
                        emptyValues.push_back(0.0);
                        break;
                    case DataType::STRING:
                        emptyValues.push_back(std::string(""));
                        break;
                }
            }
            Record record(0,schema,emptyValues) ;
            if(!table.getRecord(id,record)){
                continue ; 
            }
           bool matches = false ;
           if(query.condition.op=="="){
               matches = (record.getValue(columnIndex)==expected) ;
           }
           else if(isIntegerColumn){
            std::int64_t actual  =std::get<std::int64_t>(record.getValue(columnIndex)) ;
            std::int64_t target = std::get<std::int64_t>(expected) ; 
            if(query.condition.op==">"){
                    matches = (actual>target) ; 
                }
                else if(query.condition.op=="<"){
                    matches = (actual<target) ; 
                } 
           }
           if(matches){
                results.push_back(record) ; 
           }
        }
        return results ;
    }
};
} // namespace minidb

#endif
