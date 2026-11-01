#include "index_manager.h"
#include "memory_table.h"
#include <cassert>
#include <iostream>

using namespace minidb;
int main() {
    ::std::cout << "Starting IndexManager tests...\n";
            
    // Create schema
    Schema schema;
    schema.addColumn(Column("age", DataType::INT));
    schema.addColumn(Column("name", DataType::STRING));

    // Create table and manager
    MemoryTable table;
    IndexManager manager(schema, "age");

    // Create records
    Record r1(101, schema, {
        ::std::int64_t(20),
        ::std::string("Alice")
    });

    Record r2(102, schema, {
        ::std::int64_t(25),
        ::std::string("Bob")
    });

    Record r3(103, schema, {
        ::std::int64_t(20),
        ::std::string("Charlie")
    });

    // Test insertion
    manager.insertRecord(table, r1);
    manager.insertRecord(table, r2);
    manager.insertRecord(table, r3);

    assert(table.getALLRecordIds().size() == 3);
    assert(manager.getHashIndex()->find(20).size() == 2);
    assert(manager.getAVLIndex()->find(25).size() == 1);

    ::std::cout << "Record insertion and indexing passed!\n";

    // Test replacement of an existing record
    Record updatedR1(101, schema, {
        ::std::int64_t(30),
        ::std::string("Alice")
    });

    manager.insertRecord(table, updatedR1);

    assert(manager.getHashIndex()->find(20).size() == 1);
    assert(manager.getHashIndex()->find(30).size() == 1);
    assert(manager.getAVLIndex()->find(20).size() == 1);
    assert(manager.getAVLIndex()->find(30).size() == 1);

    ::std::cout << "Record replacement and index updates passed!\n";

    // Test deletion
    bool deleted = manager.deleteRecord(table, 102);

    assert(deleted);
    assert(table.getALLRecordIds().size() == 2);
    assert(manager.getHashIndex()->find(25).empty());
    assert(manager.getAVLIndex()->find(25).empty());

    ::std::cout << "Record deletion and index updates passed!\n";

    // Test deleting a nonexistent record
    assert(!manager.deleteRecord(table, 999));

    ::std::cout << "Nonexistent record deletion test passed!\n";

    ::std::cout << "\nAll IndexManager tests passed!\n";

    return 0;
}
