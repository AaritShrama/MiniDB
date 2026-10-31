// THIS FILE CONTAINS THE DEFINITIONS OF A COLUMN

#ifndef MINIDB_COLUMN_H
#define MINIDB_COLUMN_H
#include <string>
#include "value.h"
namespace minidb{
    // COLUMN IS ASSOCIATED TO ITS NAME AND DTYPE
    struct Column{
        std::string name;
        DataType type;
        // CONSTRUCTOR - self.name = columnName & self.type = columnType (In Python Style)
        Column(const std::string& columnName, DataType columnType): name(columnName), type(columnType){}
    };
}
#endif