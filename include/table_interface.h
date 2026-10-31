#ifndef MINIDB_TABLE_INTERFACE_H
#define MINIDB_TABLE_INTERFACE_H

#include "record.h"
#include <vector>

namespace minidb {

class TableInterface {
public:
    virtual ~TableInterface() = default;
    virtual bool getRecord(RecordId id, Record& result) const = 0;
    virtual std::vector<RecordId> getALLRecordIds() const = 0;
};

} // namespace minidb

#endif