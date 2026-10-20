// IN THIS FILE WE DEFINE A HEADER CONTAINING ALL NEEDED DATATYPES IN THE DBMS

#ifndef MINIDB_VALUE_H
#define MINIDB_VALUE_H

#include <cstdint> // HAS INT64 DTYPE
#include <string>
#include <variant> 

namespace minidb{
    enum class DataType{INT, FLOAT, STRING}; // CAN ONLY BE ONE OF THE FOLLOWING

    using Value = std::variant<std::int64_t,double,std::string>; // A VARIABLE WHICH CAN BE EITHER OF THE THREE DTYPES (VARIANT)

    // ENTER VALUE AND GET IT'S DTYPE
    inline DataType getDataType(const Value& value){
        if(std::holds_alternative<std::int64_t>(value)){return DataType::INT;}
        if(std::holds_alternative<double>(value)){return DataType::FLOAT;}
        return DataType::STRING;
    }

    // ENTER DTYPE IN DBMS FORMAT AND GET ITS NAME AS A STRING
    inline std::string dataTypeName(DataType type){
        switch(type){
            case DataType::INT :
                return "INT";
            
            case DataType::FLOAT :
                return "FLOAT";
            
            case DataType::STRING :
                return "STRING";
            }
        return "UNKNOWN";
    }
    
}

#endif