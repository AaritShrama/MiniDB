
#include "record.h"

#include <stdexcept>
#include <utility>

namespace minidb {

Record::Record(
    RecordId id,
    const Schema& schema,
    std::vector<Value> recordValues
) : recordId(id), values(std::move(recordValues)) {

    if (values.size() != schema.columnCount()) {
        throw std::invalid_argument(
            "Record value count does not match schema"
        );
    }

    for (std::size_t i = 0; i < values.size(); i++) {
        if (getDataType(values[i]) != schema.getColumn(i).type) {
            throw std::invalid_argument(
                "Data type mismatch at column: " +
                schema.getColumn(i).name
            );
        }
    }
}

RecordId Record::getId() const {
    return recordId;
}

std::size_t Record::size() const {
    return values.size();
}

const Value& Record::getValue(std::size_t index) const {
    return values.at(index);
}

} // namespace minidb
