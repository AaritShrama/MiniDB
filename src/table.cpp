
#include "table.h"

#include <cstdint>
#include <limits>
#include <stdexcept>
#include <utility>

namespace minidb {

Table::Table(
    const std::string& tableName,
    const Schema& tableSchema
) : name(tableName), schema(tableSchema) {}

const std::string& Table::getName() const {
    return name;
}

const Schema& Table::getSchema() const {
    return schema;
}

RecordId Table::insertRecord(std::vector<Value> values) {
    if (nextRecordId == 0 ||
        nextRecordId == std::numeric_limits<RecordId>::max()) {
        throw std::overflow_error("RecordId limit reached");
    }

    Record record(nextRecordId, schema, std::move(values));

    records.push_back(std::move(record));

    return nextRecordId++;
}

void Table::restoreRecord(RecordId id, std::vector<Value> values) {
    if (id == 0) {
        throw std::invalid_argument("RecordId cannot be zero");
    }

    if (findRecord(id) != nullptr) {
        throw std::invalid_argument("Duplicate RecordId");
    }

    if (id >= nextRecordId &&
        id == std::numeric_limits<RecordId>::max()) {
        throw std::overflow_error("RecordId limit reached");
    }

    Record record(id, schema, std::move(values));

    records.push_back(std::move(record));

    if (id >= nextRecordId) {
        nextRecordId = id + 1;
    }
}

const std::vector<Record>& Table::getRecords() const {
    return records;
}

RecordId Table::getNextRecordId() const {
    return nextRecordId;
}

void Table::restoreNextRecordId(RecordId nextId) {
    if (nextId == 0 || nextId < nextRecordId) {
        throw std::invalid_argument("Invalid next RecordId");
    }

    nextRecordId = nextId;
}

const Record* Table::findRecord(RecordId id) const {
    for (const Record& record : records) {
        if (record.getId() == id) {
            return &record;
        }
    }

    return nullptr;
}

bool Table::deleteRecord(RecordId id) {
    for (auto it = records.begin(); it != records.end(); ++it) {
        if (it->getId() == id) {
            records.erase(it);
            return true;
        }
    }

    return false;
}

std::size_t Table::recordCount() const {
    return records.size();
}

} // namespace minidb
