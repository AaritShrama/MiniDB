#include "record.h"

#include <stdexcept>
#include <utility>

namespace minidb {

Record::Record(
	RecordId id,
	const Schema& schema,
	std::vector<Value> recordValues
)
	: recordId(id), values(std::move(recordValues)) {
	if (values.size() != schema.columnCount()) {
		throw std::invalid_argument("Record does not match schema");
	}

	for (std::size_t index = 0; index < values.size(); ++index) {
		if (getDataType(values[index]) != schema.getColumn(index).type) {
			throw std::invalid_argument(
				"Record data type does not match schema"
			);
		}
	}
}

RecordId Record::getId() const {
	return recordId;
}

std::size_t Record::size() const {
	return values.size();
}

const Value& Record::getValue(std::size_t index) const {
	return values.at(index);
}

} // namespace minidb
