
#include "database.h"
#include "storage.h"

#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <system_error>
#include <utility>
#include <cstdint>

namespace minidb {

namespace {

constexpr std::uint64_t FORMAT_VERSION = 1;
constexpr std::uint64_t MAX_TABLES = 100000;
constexpr std::uint64_t MAX_COLUMNS = 100000;
constexpr std::uint64_t MAX_RECORDS = 10000000;
constexpr std::uint64_t MAX_VALUES = 100000;

void writeHeader(std::ostream& out) {
    storage::writeString(out, "MINIDB");
    storage::writeUInt64(out, FORMAT_VERSION);
}

void readHeader(std::istream& in) {
    if (storage::readString(in) != "MINIDB") {
        throw std::runtime_error("Invalid database file");
    }

    if (storage::readUInt64(in) != FORMAT_VERSION) {
        throw std::runtime_error(
            "Unsupported database file version"
        );
    }
}

std::uint64_t dataTypeToNumber(DataType type) {
    switch (type) {
        case DataType::INT:
            return 1;

        case DataType::FLOAT:
            return 2;

        case DataType::STRING:
            return 3;
    }

    throw std::runtime_error("Unknown column data type");
}

DataType numberToDataType(std::uint64_t type) {
    switch (type) {
        case 1:
            return DataType::INT;

        case 2:
            return DataType::FLOAT;

        case 3:
            return DataType::STRING;

        default:
            throw std::runtime_error(
                "Invalid column type in database file"
            );
    }
}

void writeColumn(
    std::ostream& out,
    const Column& column
) {
    storage::writeString(out, column.name);
    storage::writeUInt64(out, dataTypeToNumber(column.type));
}

Column readColumn(std::istream& in) {
    std::string name = storage::readString(in);
    DataType type = numberToDataType(
        storage::readUInt64(in)
    );

    return Column(name, type);
}

} // anonymous namespace


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


// Save the complete database to disk.
void Database::saveToFile(
    const std::filesystem::path& filePath
) const {
    if (filePath.empty()) {
        throw std::invalid_argument(
            "Database file path cannot be empty"
        );
    }

    std::filesystem::path tempPath = filePath;
    tempPath += ".tmp";

    std::filesystem::path backupPath = filePath;
    backupPath += ".bak";

    if (!filePath.parent_path().empty()) {
        std::filesystem::create_directories(
            filePath.parent_path()
        );
    }

    // Write to a temporary file first.
    {
        std::ofstream out(
            tempPath,
            std::ios::binary | std::ios::trunc
        );

        if (!out) {
            throw std::runtime_error(
                "Could not open temporary database file"
            );
        }

        writeHeader(out);

        storage::writeString(out, name);
        storage::writeUInt64(
            out,
            static_cast<std::uint64_t>(tables.size())
        );

        for (const auto& entry : tables) {
            const Table& table = entry.second;
            const Schema& schema = table.getSchema();

            // Table name
            storage::writeString(out, table.getName());

            // Schema
            storage::writeUInt64(
                out,
                static_cast<std::uint64_t>(
                    schema.columnCount()
                )
            );

            for (std::size_t i = 0;
                 i < schema.columnCount(); ++i) {
                writeColumn(out, schema.getColumn(i));
            }

            // Next internal RecordId
            storage::writeUInt64(
                out,
                table.getNextRecordId()
            );

            // Records
            const auto& records = table.getRecords();

            storage::writeUInt64(
                out,
                static_cast<std::uint64_t>(records.size())
            );

            for (const Record& record : records) {
                storage::writeUInt64(
                    out,
                    record.getId()
                );

                storage::writeUInt64(
                    out,
                    static_cast<std::uint64_t>(record.size())
                );

                for (std::size_t i = 0;
                     i < record.size(); ++i) {
                    storage::writeValue(
                        out,
                        record.getValue(i)
                    );
                }
            }
        }

        out.flush();

        if (!out) {
            throw std::runtime_error(
                "Failed to write database file"
            );
        }

        out.close();

        if (!out) {
            throw std::runtime_error(
                "Failed to close database file"
            );
        }
    }

    // Preserve the previous save while replacing it.
    std::error_code ec;
    bool hadOriginal = std::filesystem::exists(filePath);

    if (hadOriginal) {
        std::filesystem::remove(backupPath, ec);
        ec.clear();

        std::filesystem::rename(
            filePath,
            backupPath,
            ec
        );

        if (ec) {
            std::filesystem::remove(tempPath);
            throw std::runtime_error(
                "Could not back up existing database file"
            );
        }
    }

    std::filesystem::rename(tempPath, filePath, ec);

    if (ec) {
        // Attempt to restore the previous save.
        if (hadOriginal) {
            std::error_code restoreError;

            std::filesystem::rename(
                backupPath,
                filePath,
                restoreError
            );
        }

        std::filesystem::remove(tempPath);

        throw std::runtime_error(
            "Could not replace database file"
        );
    }

    // The new file is installed; the backup is no longer needed.
    if (hadOriginal) {
        std::filesystem::remove(backupPath, ec);
    }
}


// Load a database from disk.
Database Database::loadFromFile(
    const std::filesystem::path& filePath
) {
    std::ifstream in(
        filePath,
        std::ios::binary
    );

    if (!in) {
        throw std::runtime_error(
            "Could not open database file"
        );
    }

    readHeader(in);

    std::string databaseName =
        storage::readString(in);

    Database db(databaseName);

    const std::uint64_t tableCount =
        storage::readUInt64(in);

    if (tableCount > MAX_TABLES) {
        throw std::runtime_error(
            "Too many tables in database file"
        );
    }

    for (std::uint64_t t = 0; t < tableCount; ++t) {
        std::string tableName =
            storage::readString(in);

        const std::uint64_t columnCount =
            storage::readUInt64(in);

        if (columnCount > MAX_COLUMNS) {
            throw std::runtime_error(
                "Too many columns in database file"
            );
        }

        Schema schema;

        for (std::uint64_t c = 0; c < columnCount; ++c) {
            schema.addColumn(readColumn(in));
        }

        db.createTable(tableName, schema);

        Table* table = db.getTable(tableName);

        const std::uint64_t savedNextId =
            storage::readUInt64(in);

        if (savedNextId == 0) {
            throw std::runtime_error(
                "Invalid next RecordId in database file"
            );
        }

        const std::uint64_t recordCount =
            storage::readUInt64(in);

        if (recordCount > MAX_RECORDS) {
            throw std::runtime_error(
                "Too many records in database file"
            );
        }

        for (std::uint64_t r = 0; r < recordCount; ++r) {
            const RecordId id =
                storage::readUInt64(in);

            const std::uint64_t valueCount =
                storage::readUInt64(in);

            if (valueCount > MAX_VALUES ||
                valueCount != columnCount) {
                throw std::runtime_error(
                    "Invalid record value count"
                );
            }

            std::vector<Value> values;
            values.reserve(
                static_cast<std::size_t>(valueCount)
            );

            for (std::uint64_t v = 0; v < valueCount; ++v) {
                values.push_back(
                    storage::readValue(in)
                );
            }

            // restoreRecord validates types and preserves IDs.
            table->restoreRecord(id, std::move(values));
        }

        // Restore the saved counter after restoring all records.
        table->restoreNextRecordId(savedNextId);
    }

    // Reject unexpected trailing data.
    if (in.peek() != std::char_traits<char>::eof()) {
        throw std::runtime_error(
            "Unexpected trailing data in database file"
        );
    }

    if (in.bad()) {
        throw std::runtime_error(
            "Error reading database file"
        );
    }

    return db;
}

} // namespace minidb
