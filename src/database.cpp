
#include "database.h"

#include <stdexcept>
#include <utility>

namespace minidb {

Database::Database(const std::string& databaseName)
    : name(databaseName) {}

const std::string& Database::getName() const {
    return name;
}

void Database::createTable(
    const std::string& tableName,
    const Schema& schema
) {
    auto result = tables.try_emplace(
        tableName,
        tableName,
        schema
    );

    if (!result.second) {
        throw std::invalid_argument(
            "Table already exists: " + tableName
        );
    }
}

Table* Database::getTable(const std::string& tableName) {
    auto it = tables.find(tableName);

    if (it == tables.end()) {
        return nullptr;
    }

    return &it->second;
}

const Table* Database::getTable(
    const std::string& tableName
) const {
    auto it = tables.find(tableName);

    if (it == tables.end()) {
        return nullptr;
    }

    return &it->second;
}

bool Database::dropTable(const std::string& tableName) {
    return tables.erase(tableName) > 0;
}

std::size_t Database::tableCount() const {
    return tables.size();
}

} // namespace minidb
