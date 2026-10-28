
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
#include "database.h"

int main() {
    std::cout << "MiniDB setup successful!\n";
    try {
        const std::filesystem::path filePath = "minidb_test.db";

        // 1. Create a database and table
        minidb::Database db("CollegeDB");

        minidb::Schema schema;
        schema.addColumn(
            minidb::Column("id", minidb::DataType::INT)
        );
        schema.addColumn(
            minidb::Column("name", minidb::DataType::STRING)
        );

        db.createTable("Students", schema);

        // 2. Insert records
        minidb::Table* students = db.getTable("Students");

        students->insertRecord({
            std::int64_t{101},
            std::string{"Aarit"}
        });

        students->insertRecord({
            std::int64_t{102},
            std::string{"Rahul"}
        });

        std::cout << "Before saving:\n";
        std::cout << "Database: " << db.getName() << '\n';
        std::cout << "Tables: " << db.tableCount() << '\n';
        std::cout << "Records: " << students->recordCount() << "\n\n";

        // 3. Save database to file
        db.saveToFile(filePath);
        std::cout << "Database saved successfully.\n";

        // 4. Load database from file
        minidb::Database loadedDb =
            minidb::Database::loadFromFile(filePath);

        // 5. Verify loaded data
        const minidb::Table* loadedStudents =
            loadedDb.getTable("Students");

        if (loadedStudents == nullptr) {
            std::cout << "ERROR: Students table not found!\n";
            return 1;
        }

        std::cout << "\nAfter loading:\n";
        std::cout << "Database: " << loadedDb.getName() << '\n';
        std::cout << "Tables: " << loadedDb.tableCount() << '\n';
        std::cout << "Records: "
                  << loadedStudents->recordCount() << '\n';

        for (const minidb::Record& record :
             loadedStudents->getRecords()) {
            std::cout << "Record ID: " << record.getId()
                      << " | Student ID: "
                      << std::get<std::int64_t>(record.getValue(0))
                      << " | Name: "
                      << std::get<std::string>(record.getValue(1))
                      << '\n';
        }

        std::cout << "\nPersistence test passed!\n";
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}