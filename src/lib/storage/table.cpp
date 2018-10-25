#include "table.hpp"

#include <algorithm>
#include <iomanip>
#include <limits>
#include <memory>
#include <numeric>
#include <string>
#include <utility>
#include <vector>

#include "value_segment.hpp"

#include "resolve_type.hpp"
#include "types.hpp"
#include "utils/assert.hpp"

namespace opossum {

Table::Table(const uint32_t chunk_size) {
  this->max_chunk_size = chunk_size;
  this->chunks = std::vector<std::shared_ptr<Chunk>>();
  this->chunks.push_back(std::make_shared<Chunk>());
  this->names = std::vector<std::string>();
  this->types = std::vector<std::string>();
}

void Table::add_column(const std::string& name, const std::string& type) {
  this->names.push_back(name);
  this->types.push_back(type);

  if (this->chunks.front()->size() == 0) {
    this->chunks.back()->add_segment(make_shared_by_data_type<BaseSegment, ValueSegment>(type));
  } else {
    throw std::runtime_error("Error: Tried to add column to a non empty table");
  }
}

void Table::append(std::vector<AllTypeVariant> values) {
  if (this->chunks.back()->size() >= this->max_chunk_size) {
    this->chunks.push_back(std::make_shared<Chunk>());
    for (uint16_t i = 0; i < this->names.size(); i ++) {
      this->chunks.back()->add_segment(
        make_shared_by_data_type<BaseSegment, ValueSegment>(this->types[i])
      );
    }
  }
  this->chunks.back()->append(values);
}

uint16_t Table::column_count() const {
  return this->names.size();
}

uint64_t Table::row_count() const {
  uint64_t count = 0;
  // vector.size() has constant complexity
  for (uint64_t i = 0; i < this->chunks.size(); i ++) {
    count += this->chunks[i]->size();
  }
  return count;
}

ChunkID Table::chunk_count() const {
  return ChunkID{(uint32_t)this->chunks.size()};
}

ColumnID Table::column_id_by_name(const std::string& column_name) const {
  for (uint16_t i = 0; i < this->names.size(); i++) {
    if (this->names[i].compare(column_name) == 0) {
      return ColumnID{i};
    }
  }
  throw std::runtime_error("Column does not exist");
}

uint32_t Table::chunk_size() const {
  return this->max_chunk_size;
}

const std::vector<std::string>& Table::column_names() const {
  return this->names;
}

const std::string& Table::column_name(ColumnID column_id) const {
  return this->names[column_id];
}

const std::string& Table::column_type(ColumnID column_id) const {
  return this->types[column_id];
}

Chunk& Table::get_chunk(ChunkID chunk_id) { 
  return *this->chunks[chunk_id];
}

const Chunk& Table::get_chunk(ChunkID chunk_id) const { 
  return *this->chunks[chunk_id];
}

}  // namespace opossum
