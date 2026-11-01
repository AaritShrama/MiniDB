#ifndef MINIDB_TABLE_H
#define MINIDB_TABLE_H

#include <string>
#include <vector>
#include "record.h"
#include "schema.h"
#include "table_interface.h"

namespace minidb{
    class Table : public TableInterface {
        private:
            std::string name;
            Schema schema;
            std::vector<Record> records;
            RecordId nextRecordId = 1;

        public:
            Table(const std::string& tableName, const Schema& tableSchema);
            const std::string& getName() const;
            const Schema& getSchema() const;
            RecordId insertRecord(std::vector<Value> values);
            void restoreRecord(RecordId id, std::vector<Value> values);
            const std::vector<Record>& getRecords() const;
            RecordId getNextRecordId() const;
            void restoreNextRecordId(RecordId nextId);
            const Record* findRecord(RecordId id) const;
          
            std::size_t recordCount() const;
            bool getRecord(RecordId id, Record& result) const override;
            std::vector<RecordId> getALLRecordIds() const override;
            void insertRecord(const Record& record) override;
            bool deleteRecord(RecordId id) override;
    };
}

#endif