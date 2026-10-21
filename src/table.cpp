#include "table.h"
#include <stdexcept>
#include <utility>

namespace minidb{
    Table::Table(const std::string& tableName,const Schema& tableSchema):
    name(tableName), schema(tableSchema){}

    const std::string& Table::getName() const {
        return name;
    }

    const Schema& Table::getSchema() const {
        return schema;
    }

    void Table::insertRecord(const Record& record) {
        if (record.size() != schema.columnCount()) {
            throw std::invalid_argument(
                "Record does not match table schema"
            );
        }

        for (std::size_t i = 0; i < record.size(); i++) {
            if (getDataType(record.getValue(i)) != schema.getColumn(i).type) {
                throw std::invalid_argument(
                    "Record data type does not match table schema"
                );
            }
        }

        if (findRecord(record.getId()) != nullptr) {
            throw std::invalid_argument(
                "Duplicate internal RecordId"
            );
        }

        records.push_back(record);
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

    
}
