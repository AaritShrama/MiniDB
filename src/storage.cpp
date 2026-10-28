
#include "storage.h"
#include "value.h"

#include <cstdint>
#include <cstring>
#include <istream>
#include <ostream>
#include <stdexcept>
#include <string>
#include <variant>
#include <limits>

namespace minidb::storage {

// Write a 64-bit unsigned integer in little-endian format.
void writeUInt64(std::ostream& out, std::uint64_t value) {
    for (int i = 0; i < 8; ++i) {
        out.put(static_cast<char>(value & 0xFF));
        value >>= 8;
    }

    if (!out) {
        throw std::runtime_error("Failed to write integer");
    }
}

// Read a 64-bit unsigned integer in little-endian format.
std::uint64_t readUInt64(std::istream& in) {
    std::uint64_t value = 0;

    for (int i = 0; i < 8; ++i) {
        int byte = in.get();

        if (byte == std::char_traits<char>::eof()) {
            throw std::runtime_error("Unexpected end of file");
        }

        value |= static_cast<std::uint64_t>(
            static_cast<unsigned char>(byte)
        ) << (8 * i);
    }

    return value;
}

// Write a string as: length followed by its bytes.
void writeString(std::ostream& out, const std::string& value) {
    writeUInt64(out, static_cast<std::uint64_t>(value.size()));

    out.write(
        value.data(),
        static_cast<std::streamsize>(value.size())
    );

    if (!out) {
        throw std::runtime_error("Failed to write string");
    }
}

// Read a length-prefixed string.
std::string readString(std::istream& in) {
    const std::uint64_t length = readUInt64(in);

    // Reject unreasonably large strings.
    if (length > 16ULL * 1024 * 1024) {
        throw std::runtime_error("String length is too large");
    }

    std::string value(static_cast<std::size_t>(length), '\0');

    in.read(
        value.data(),
        static_cast<std::streamsize>(length)
    );

    if (!in) {
        throw std::runtime_error("Failed to read string");
    }

    return value;
}

// Write a Value with a type marker:
// 1 = INT, 2 = FLOAT, 3 = STRING.
void writeValue(std::ostream& out, const Value& value) {
    if (std::holds_alternative<std::int64_t>(value)) {
        out.put(1);

        const std::int64_t number =
            std::get<std::int64_t>(value);

        std::uint64_t bits = 0;

        static_assert(sizeof(number) == sizeof(bits));

        std::memcpy(&bits, &number, sizeof(bits));

        writeUInt64(out, bits);
    }
    else if (std::holds_alternative<double>(value)) {
        out.put(2);

        const double number = std::get<double>(value);
        std::uint64_t bits = 0;

        static_assert(sizeof(number) == sizeof(bits));

        std::memcpy(&bits, &number, sizeof(bits));

        writeUInt64(out, bits);
    }
    else if (std::holds_alternative<std::string>(value)) {
        out.put(3);

        writeString(
            out,
            std::get<std::string>(value)
        );
    }
    else {
        throw std::runtime_error("Unsupported Value type");
    }

    if (!out) {
        throw std::runtime_error("Failed to write value");
    }
}

// Read a Value using its stored type marker.
Value readValue(std::istream& in) {
    const int type = in.get();

    if (type == std::char_traits<char>::eof()) {
        throw std::runtime_error("Unexpected end of file");
    }

    if (type == 1) {
        const std::uint64_t bits = readUInt64(in);

        std::int64_t number = 0;

        static_assert(sizeof(number) == sizeof(bits));

        std::memcpy(&number, &bits, sizeof(number));

        return number;
    }

    if (type == 2) {
        const std::uint64_t bits = readUInt64(in);

        double number = 0.0;

        static_assert(sizeof(number) == sizeof(bits));

        std::memcpy(&number, &bits, sizeof(number));

        return number;
    }

    if (type == 3) {
        return readString(in);
    }

    throw std::runtime_error("Unknown value type in database file");
}

} 
