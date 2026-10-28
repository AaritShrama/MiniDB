
#ifndef MINIDB_DATABASE_H
#define MINIDB_DATABASE_H

#include <cstddef>
#include <filesystem>
#include <map>
#include <string>

#include "table.h"
#include "schema.h"

namespace minidb {

class Database {
private:
    std::string name;
    std::map<std::string, Table> tables;

public:
    explicit Database(const std::string& databaseName);

    const std::string& getName() const;

    void createTable(
        const std::string& tableName,
        const Schema& schema
    );

    Table* getTable(const std::string& tableName);

    const Table* getTable(const std::string& tableName) const;

    bool dropTable(const std::string& tableName);

    std::size_t tableCount() const;

    // Persistence
    void saveToFile(
        const std::filesystem::path& filePath
    ) const;

    static Database loadFromFile(
        const std::filesystem::path& filePath
    );
};

} // namespace minidb

#endif
