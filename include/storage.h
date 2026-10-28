
#ifndef MINIDB_STORAGE_H
#define MINIDB_STORAGE_H

#include <cstdint>
#include <iosfwd>
#include <string>

#include "value.h"

namespace minidb::storage {

void writeUInt64(std::ostream& out, std::uint64_t value);
std::uint64_t readUInt64(std::istream& in);

void writeString(std::ostream& out, const std::string& value);
std::string readString(std::istream& in);

void writeValue(std::ostream& out, const Value& value);
Value readValue(std::istream& in);

} // namespace minidb::storage

#endif
