
#include "query_engine.h"
#include "memory_table.h"

#include <cassert>
#include <iostream>

using namespace minidb;

int main() {

    std::cout << "Starting Query Engine tests...\n";

    // Create schema.
    Schema schema;

    schema.addColumn(Column("age", DataType::INT));
    schema.addColumn(Column("marks", DataType::FLOAT));
    schema.addColumn(Column("name", DataType::STRING));

    // Create records.
    Record r1(101, schema, {
        std::int64_t(20), 85.5, std::string("Alice")
    });

    Record r2(102, schema, {
        std::int64_t(21), 92.0, std::string("Bob")
    });

    Record r3(103, schema, {
        std::int64_t(20), 78.5, std::string("Charlie")
    });

    // Insert into MemoryTable.
    MemoryTable table;

    table.insertRecord(r1);
    table.insertRecord(r2);
    table.insertRecord(r3);

    // Test 1: SELECT all records.
    auto allRecords = QueryEngine::execute(
        "SELECT * FROM students;",
        table,
        schema
    );

    assert(allRecords.size() == 3);

    std::cout << "SELECT all test passed!\n";

    // Test 2: Integer equality condition.
    auto ageResults = QueryEngine::execute(
        "SELECT * FROM students WHERE age = 20;",
        table,
        schema
    );

    assert(ageResults.size() == 2);

    std::cout << "Integer WHERE test passed!\n";

    // Test 3: Float equality condition.
    auto marksResults = QueryEngine::execute(
        "SELECT * FROM students WHERE marks = 92.0;",
        table,
        schema
    );

    assert(marksResults.size() == 1);
    assert(marksResults[0].getId() == 102);

    std::cout << "Float WHERE test passed!\n";

    // Test 4: String equality condition.
    auto nameResults = QueryEngine::execute(
        "SELECT * FROM students WHERE name = 'Alice';",
        table,
        schema
    );

    assert(nameResults.size() == 1);
    assert(nameResults[0].getId() == 101);

    std::cout << "String WHERE test passed!\n";

    std::cout << "\nAll Query Engine tests passed!\n";

    return 0;
}
