#include <iostream>
#include <string>
#include <vector>

#include "record.h"

int main() {
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

    minidb::Record student(
        5001,
        studentSchema,
        std::vector<minidb::Value>{
            std::int64_t{101},
            std::string{"Aarit"},
            8.4
        }
    );

    std::cout << "Internal Record ID: "
              << student.getId() << '\n';

    for (std::size_t i = 0; i < student.size(); i++) {
        const minidb::Column& column =
            studentSchema.getColumn(i);

        const minidb::Value& value = student.getValue(i);

        std::cout << column.name << " : ";

        if (std::holds_alternative<std::int64_t>(value)) {
            std::cout << std::get<std::int64_t>(value);
        } else if (std::holds_alternative<double>(value)) {
            std::cout << std::get<double>(value);
        } else {
            std::cout << std::get<std::string>(value);
        }

        std::cout << '\n';
    }

    return 0;
}
