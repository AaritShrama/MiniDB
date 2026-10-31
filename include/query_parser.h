
#ifndef MINIDB_QUERY_PARSER_H
#define MINIDB_QUERY_PARSER_H

#include <string>
#include <vector>
#include <stdexcept>
#include <cctype>

namespace minidb {

struct Condition {
    std::string column;
    std::string op;
    std::string value;
};

struct ParsedQuery {
    std::string tableName;
    std::vector<std::string> selectedColumns;
    Condition condition;
    bool hasCondition = false;
};

class QueryParser {
public:
    static ParsedQuery parse(const std::string& sql) {
        ParsedQuery query;
        std::string input = sql;

        // Convert SQL keywords to uppercase.
        // This first version assumes identifiers and values
        // do not require case-sensitive handling.
        for (char& ch : input) {
            if (ch == ';') {
                ch = ' ';
            }
        }

        std::vector<std::string> tokens;
        std::string token;

        for (char ch : input) {
            if (std::isspace(static_cast<unsigned char>(ch))) {
                if (!token.empty()) {
                    tokens.push_back(token);
                    token.clear();
                }
            } else if (ch == '=' || ch == '<' || ch == '>') {
                if (!token.empty()) {
                    tokens.push_back(token);
                    token.clear();
                }
                tokens.push_back(std::string(1, ch));
            } else {
                token += ch;
            }
        }

        if (!token.empty()) {
            tokens.push_back(token);
        }

        if (tokens.size() < 4 ||
            tokens[0] != "SELECT" ||
            tokens[2] != "FROM") {
            throw std::invalid_argument("Invalid SELECT query");
        }

        // Selected columns
        if (tokens[1] == "*") {
            query.selectedColumns.push_back("*");
        } else {
            query.selectedColumns.push_back(tokens[1]);
        }

        query.tableName = tokens[3];

        // Optional WHERE condition
        if (tokens.size() > 4) {
            if (tokens.size() != 8 || tokens[4] != "WHERE") {
                throw std::invalid_argument(
                    "Unsupported WHERE condition"
                );
            }

            query.condition.column = tokens[5];
            query.condition.op = tokens[6];
            query.condition.value = tokens[7];
            query.hasCondition = true;
        }

        return query;
    }
};

} // namespace minidb

#endif
