#ifndef MINIDB_QUERY_ENGINE_H
#define MINIDB_QUERY_ENGINE_H  
#include "query_parser.h"
#include "table_interface.h"
#include "schema.h"
#include <vector>
#include <string>
#include <stdexcept>
#include <cstdint>
namespace minidb {
class QueryEngine {
public:
    static std::vector<Record> execute(
        const std::string& sql,
        const TableInterface& table,
        const Schema& schema
    ) {
        ParsedQuery query = QueryParser::parse(sql);
        if (query.selectedColumns.size() != 1 ||
            query.selectedColumns[0] != "*") {
            throw std::invalid_argument(
                "Currently only SELECT * is supported"
            );
        }
        std::vector<Record> results;
        std::vector<RecordId> ids = table.getALLRecordIds();
        // SELECT * FROM students
        if (!query.hasCondition) {
            for (RecordId id : ids) {
                Record record;
                if (table.getRecord(id, record)) {
                    results.push_back(record);
                }
            }
            return results;
        }
        // Currently supporting equality conditions only.
        if (query.condition.op != "=") {
            throw std::invalid_argument(
                "Currently only equality (=) is supported"
            );
        }
        // Find the column index.
        std::size_t columnIndex = schema.columnCount();
        bool found = false;

        for (std::size_t i = 0; i < schema.columnCount(); i++) {
            if (schema.getColumn(i).name == query.condition.column) {
                columnIndex = i;
                found = true;
                break;
            }
        }
        if (!found) {
            throw std::invalid_argument(
                "Unknown column: " + query.condition.column
            );
        }
        // Convert the condition value to the correct data type.
        Value expected;
        switch (schema.getColumn(columnIndex).type) {
            case DataType::INT:
                expected = static_cast<std::int64_t>(
                    std::stoll(query.condition.value)
                );
                break;
            case DataType::FLOAT:
                expected = std::stod(query.condition.value);
                break;
            case DataType::STRING: {
                std::string val = query.condition.value;
                if (val.size() >= 2 && ((val.front() == '\'' && val.back() == '\'') ||
                                        (val.front() == '"' && val.back() == '"'))) {
                    val = val.substr(1, val.size() - 2);
                }
                expected = val;
                break;
            }
        }
        // Search records using a table scan.
        for (RecordId id : ids) {
            Record record;
            if (!table.getRecord(id, record)) {
                continue;
            }
            if (record.getValue(columnIndex) == expected) {
                results.push_back(record);
            }
        }
        return results;
    }
};
} // namespace minidb

#endif
