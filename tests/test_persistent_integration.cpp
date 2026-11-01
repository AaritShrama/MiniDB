
#include <iostream>
#include <cassert>
#include <cstdint>
#include <vector>

#include "database.h"
#include "index_manager.h"
#include "query_engine.h"

using namespace minidb;

int main() {
    std::cout << "Starting Persistent Integration Tests...\n";

    // 1. Create schema
    Schema schema;
    schema.addColumn(Column("id", DataType::INT));
    schema.addColumn(Column("age", DataType::INT));

    // 2. Create database and table
    Database db("TestDatabase");
    db.createTable("students", schema);

    Table* table = db.getTable("students");
    assert(table != nullptr);

    // 3. Create IndexManager
    IndexManager manager(table->getSchema(), "age");

    // 4. Insert records through IndexManager
    Record r1(1, schema, {
        std::int64_t(101),
        std::int64_t(20)
    });

    Record r2(2, schema, {
        std::int64_t(102),
        std::int64_t(22)
    });

    Record r3(3, schema, {
        std::int64_t(103),
        std::int64_t(20)
    });

    manager.insertRecord(*table, r1);
    manager.insertRecord(*table, r2);
    manager.insertRecord(*table, r3);

    assert(table->recordCount() == 3);

    std::cout << "Record insertion test passed!\n";

    // 5. Execute indexed equality query
    std::vector<Record> results = QueryEngine::execute(
        "SELECT * FROM students WHERE age = 20",
        *table,
        table->getSchema(),
        manager
    );

    assert(results.size() == 2);

    std::cout << "Indexed equality query test passed!\n";

    // 6. Save database to file
    db.saveToFile("test_minidb.db");

    std::cout << "Database save test passed!\n";

    // 7. Load database from file
    Database loadedDB = Database::loadFromFile("test_minidb.db");

    Table* loadedTable = loadedDB.getTable("students");

    assert(loadedTable != nullptr);
    assert(loadedTable->recordCount() == 3);

    std::cout << "Database load test passed!\n";

    std::cout << "\nAll Persistent Integration Tests Passed!\n";

    return 0;
}
