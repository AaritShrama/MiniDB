
#ifndef MINIDB_SCHEMA_H
#define MINIDB_SCHEMA_H

#include <cstddef>
#include <string>
#include <vector>
#include "column.h"

namespace minidb {

class Schema {
private:
    std::vector<Column> columns;

public:
    void addColumn(const Column& column);

    std::size_t columnCount() const;

    const Column& getColumn(std::size_t index) const;

    const Column* findColumn(const std::string& name) const;
};

} // namespace minidb

#endif
