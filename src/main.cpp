
#include <iostream>

#include "database.h"

int main() {
    // 1. Create a schema for the Students table
    minidb::Schema studentSchema;

    studentSchema.addColumn(
        minidb::Column("id", minidb::DataType::INT)
    );

    studentSchema.addColumn(
        minidb::Column("name", minidb::DataType::STRING)
    );

    studentSchema.addColumn(
        minidb::Column("cgpa", minidb::DataType::FLOAT)
    );

    // 2. Create the database
    minidb::Database db("MiniDB");

    // 3. Create two tables
    db.createTable("Students", studentSchema);
    db.createTable("Graduates", studentSchema);

    std::cout << "Database: " << db.getName() << '\n';
    std::cout << "Tables: " << db.tableCount() << "\n\n";

    // 4. Retrieve a table
    minidb::Table* students = db.getTable("Students");

    if (students != nullptr) {
        std::cout << "Found table: " << students->getName() << '\n';
        std::cout << "Columns: "
                  << students->getSchema().columnCount() << '\n';
    }

    // 5. Search for a table that doesn't exist
    if (db.getTable("Teachers") == nullptr) {
        std::cout << "\nTeachers table not found\n";
    }

    // 6. Drop a table
    bool dropped = db.dropTable("Graduates");

    std::cout << "\nGraduates dropped: "
              << (dropped ? "Yes" : "No") << '\n';

    std::cout << "Tables remaining: " << db.tableCount() << '\n';

    return 0;
}
