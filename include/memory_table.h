
#ifndef MINIDB_MEMORY_TABLE_H
#define MINIDB_MEMORY_TABLE_H

#include "table_interface.h"
#include <unordered_map>
#include <vector>
namespace minidb {
class MemoryTable : public TableInterface {
private:
    std::unordered_map<RecordId, Record> records;
public:
    void insertRecord(const Record& record) {
        records.insert_or_assign(record.getId(), record);
    }
    bool getRecord(RecordId id, Record& result) const override {
        auto it = records.find(id);
        if (it == records.end()) {
            return false;
        }
        result = it->second;
        return true;
    }
    std::vector<RecordId> getALLRecordIds() const override {
        std::vector<RecordId> ids;
        ids.reserve(records.size());
        for (const auto& entry : records) {
            ids.push_back(entry.first);
        }
        return ids;
    }
};
} // namespace minidb
#endif
