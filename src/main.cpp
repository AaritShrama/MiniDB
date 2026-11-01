
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
#include "database.h"
#include <iomanip>
#include <variant>
#include <cctype>

#include "index_manager.h"
#include "query_engine.h"
void displayResults(
    const std::vector<minidb::Record>& results,
    const minidb::Schema& schema
) {
    if (results.empty()) {
        std::cout << "No records found.\n";
        return;
    }

    for (std::size_t i = 0; i < schema.columnCount(); i++) {
        std::cout << std::left
                  << std::setw(15)
                  << schema.getColumn(i).name;
    }

    std::cout << '\n';
    std::cout << std::string(schema.columnCount() * 15, '-') << '\n';

    for (const auto& record : results) {
        for (std::size_t i = 0; i < record.size(); i++) {
            std::cout << std::left << std::setw(15);

            std::visit([](const auto& value) {
                std::cout << value;
            }, record.getValue(i));
        }

        std::cout << '\n';
    }

    std::cout << "\n" << results.size()
              << " record(s) found.\n";
}
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

       // Create indexes on the ID column
minidb::IndexManager manager(
    loadedStudents->getSchema(), "id"
);

// Rebuild indexes from loaded records
for (const minidb::Record& record : loadedStudents->getRecords()) {
    manager.insertRecord(*const_cast<minidb::Table*>(loadedStudents), record);
}

std::cout << "\n====================================\n";
std::cout << "       MINIDB SQL CONSOLE\n";
std::cout << "====================================\n";

std::cout << "Database loaded: " << loadedDb.getName() << '\n';
std::cout << "Indexed column: id\n";

std::cout << "\nAvailable commands:\n";
std::cout << "SELECT * FROM Students;\n";
std::cout << "SELECT * FROM Students WHERE id = 101;\n";
std::cout << "SELECT * FROM Students WHERE id > 101;\n";
std::cout << "HELP\n";
std::cout << "EXIT\n\n";

std::string sql;

while (true) {
    std::cout << "MiniDB> ";

    if (!std::getline(std::cin, sql)) {
        break;
    }

    std::string command = sql;

    for (char& ch : command) {
        ch = static_cast<char>(
            std::toupper(static_cast<unsigned char>(ch))
        );
    }

    if (command == "EXIT" || command == "EXIT;") {
        break;
    }

    if (command == "HELP" || command == "HELP;") {
        std::cout << "Enter SELECT queries or EXIT.\n";
        continue;
    }

    try {
        std::vector<minidb::Record> results =
            minidb::QueryEngine::execute(
                sql,
                *loadedStudents,
                loadedStudents->getSchema(),
                manager
            );

        displayResults(results, loadedStudents->getSchema());
    }
    catch (const std::exception& e) {
        std::cout << "Query Error: " << e.what() << '\n';
    }
}

std::cout << "\nMiniDB console closed.\n";
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}