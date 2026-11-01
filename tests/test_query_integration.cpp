#include "query_engine.h"
#include "index_manager.h"
#include "memory_table.h"

#include <cassert>
#include <iostream>
#include <cstdint>

using namespace minidb;

int main() {
    std::cout << "Starting MiniDB Integration Tests...\n";

    Schema schema;
    schema.addColumn(Column("age", DataType::INT));
    schema.addColumn(Column("name", DataType::STRING));

    MemoryTable table;
    IndexManager manager(schema, "age");

    std::cout << "Test setup completed!\n";
// Step 1: Insert records through IndexManager

Record r1(101, schema, {
    std::int64_t(20), std::string("Alice")
});

Record r2(102, schema, {
    std::int64_t(25), std::string("Bob")
});

Record r3(103, schema, {
    std::int64_t(30), std::string("Charlie")
});

manager.insertRecord(table, r1);
manager.insertRecord(table, r2);
manager.insertRecord(table, r3);

assert(table.getALLRecordIds().size() == 3);

std::cout << "Record insertion test passed!\n";

// Step 2: Test SELECT *

auto allResults = QueryEngine::execute(
    "SELECT * FROM students;",
    table,
    schema,
    manager
);

assert(allResults.size() == 3);

std::cout << "SELECT ALL test passed!\n";

// Step 3: Test equality query

auto equalityResults = QueryEngine::execute(
    "SELECT * FROM students WHERE age = 25;",
    table,
    schema,
    manager
);

assert(equalityResults.size() == 1);
assert(equalityResults[0].getId() == 102);

std::cout << "Equality query test passed!\n";

// Step 4: Test range query

auto rangeResults = QueryEngine::execute(
    "SELECT * FROM students WHERE age > 20;",
    table,
    schema,
    manager
);

assert(rangeResults.size() == 2);

std::cout << "Range query test passed!\n";
// Step 5: Test record update

Record updatedR1(101, schema, {
    std::int64_t(35), std::string("Alice")
});

manager.insertRecord(table, updatedR1);

auto oldResults = QueryEngine::execute(
    "SELECT * FROM students WHERE age = 20;",
    table, schema, manager
);

auto newResults = QueryEngine::execute(
    "SELECT * FROM students WHERE age = 35;",
    table, schema, manager
);

assert(oldResults.empty());
assert(newResults.size() == 1);
assert(newResults[0].getId() == 101);

std::cout << "Record update test passed!\n";

// Step 6: Test record deletion

bool deleted = manager.deleteRecord(table, 102);

assert(deleted);

auto deletedResults = QueryEngine::execute(
    "SELECT * FROM students WHERE age = 25;",
    table, schema, manager
);

assert(deletedResults.empty());

assert(table.getALLRecordIds().size() == 2);

std::cout << "Record deletion test passed!\n";

// Step 7: Test index consistency

assert(manager.getHashIndex()->find(25).empty());
assert(manager.getAVLIndex()->find(25).empty());

assert(manager.getHashIndex()->find(35).size() == 1);
assert(manager.getAVLIndex()->find(35).size() == 1);

std::cout << "Index consistency test passed!\n";
    return 0;
}