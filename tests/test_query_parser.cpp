#include "query_parser.h"
#include <cassert>
#include <iostream>

using namespace minidb;

int main() {
    ParsedQuery query = QueryParser::parse(
        "SELECT * FROM students WHERE age = 20;"
    );

    assert(query.tableName == "students");
    assert(query.selectedColumns.size() == 1);
    assert(query.selectedColumns[0] == "*");

    assert(query.hasCondition);
    assert(query.condition.column == "age");
    assert(query.condition.op == "=");
    assert(query.condition.value == "20");

    std::cout << "Query parser tests passed!\n";
    return 0;
}
