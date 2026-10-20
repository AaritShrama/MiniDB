#include "schema.h"
#include <stdexcept>

namespace  minidb{
    void Schema::addColumn(const Column& column){
        if(findColumn(column.name) != nullptr){
            throw std::invalid_argument("DUPLICATE COLUMN NAME : " + column.name);
        }
        columns.push_back(column);
    }

    std::size_t Schema::columnCount() const{return columns.size();}

    const Column& Schema::getColumn(std::size_t index) const{
        return columns.at(index);
    }

    const Column* Schema::findColumn(const std::string& name)const{
        for (const Column& column : columns){
            if(column.name == name){return &column;}
        }
        return nullptr;
    }

}