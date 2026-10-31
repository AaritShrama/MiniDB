
#include "memory_table.h"
#include <cassert>
#include <iostream>
#include <algorithm>

using namespace minidb;

int main() {
    std::cout << "Starting MemoryTable tests...\n";

    // Create a schema
    Schema schema;
    schema.addColumn(Column("age", DataType::INT));
    schema.addColumn(Column("marks", DataType::FLOAT));
    schema.addColumn(Column("name", DataType::STRING));

    // Create records using the existing Record constructor
    Record record1(
        101,
        schema,
        std::vector<Value>{
            std::int64_t(20),
            85.5,
            std::string("Alice")
        }
    );

    Record record2(
        102,
        schema,
        std::vector<Value>{
            std::int64_t(21),
            92.0,
            std::string("Bob")
        }
    );

    MemoryTable table;

    // Test insertion
    table.insertRecord(record1);
    table.insertRecord(record2);

    assert(table.getALLRecordIds().size() == 2);
    std::cout << "Insertion test passed!\n";

    // Test retrieval of record 101
    Record retrieved1 = record1;
    assert(table.getRecord(101, retrieved1));
    assert(retrieved1.getId() == 101);
    assert(std::get<std::string>(retrieved1.getValue(2)) == "Alice");
    std::cout << "Record retrieval test passed!\n";

    // Test retrieval of record 102
    Record retrieved2 = record2;
    assert(table.getRecord(102, retrieved2));
    assert(retrieved2.getId() == 102);
    assert(std::get<std::string>(retrieved2.getValue(2)) == "Bob");
    std::cout << "Second record retrieval test passed!\n";

    // Test missing record
    assert(!table.getRecord(999, retrieved1));
    std::cout << "Missing record test passed!\n";

    // Test returned IDs
    std::vector<RecordId> ids = table.getALLRecordIds();

    assert(std::find(ids.begin(), ids.end(), 101) != ids.end());
    assert(std::find(ids.begin(), ids.end(), 102) != ids.end());

    std::cout << "Record ID listing test passed!\n";
    std::cout << "All MemoryTable tests passed!\n";

    return 0;
}
