#ifndef MINIDB_TABLE_H
#define MINIDB_TABLE_H

#include <string>
#include <vector>
#include "record.h"
#include "schema.h"

namespace minidb{
    class Table{
        private:
            std::string name;
            Schema schema;
            std::vector<Record> records;

        public:
            Table(const std::string& tableName, const Schema& tableSchema);
            const std::string& getName() const;\
            const Schema& getSchema() const;
            void insertRecord(const Record& record);
            const Record* findRecord(RecordId id) const;
            bool deleteRecord(RecordId id);
            std::size_t recordCount() const;
    };
}

#endif