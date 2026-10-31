#include "value.h"
#include <cassert>
#include <iostream>

int main() {
    minidb::Value v1 = std::int64_t(42);
    minidb::Value v2 = 3.14;
    minidb::Value v3 = std::string("test");

    assert(minidb::getDataType(v1) == minidb::DataType::INT);
    assert(minidb::getDataType(v2) == minidb::DataType::FLOAT);
    assert(minidb::getDataType(v3) == minidb::DataType::STRING);

    assert(minidb::dataTypeName(minidb::DataType::INT) == "INT");
    assert(minidb::dataTypeName(minidb::DataType::FLOAT) == "FLOAT");
    assert(minidb::dataTypeName(minidb::DataType::STRING) == "STRING");

    std::cout << "All value.h tests passed successfully!\n";
    return 0;
}

