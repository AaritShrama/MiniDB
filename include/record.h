#ifndef MINIDB_RECORD_H
#define MINIDB_RECORD_H

#include <cstdint>
#include <vector>
#include "value.h"
#include "schema.h"

namespace minidb {

using RecordId = std::uint64_t;

class Record {
private:
    RecordId recordId = 0;
    std::vector<Value> values;

public:
    Record() = default;
    Record(RecordId id, const Schema& schema, std::vector<Value> recordValues);

    RecordId getId() const;

    std::size_t size() const;

    const Value& getValue(std::size_t index) const;
};

} // namespace minidb

#endif
