#include "table.hpp"

#include <algorithm>
#include <iomanip>
#include <limits>
#include <memory>
#include <numeric>
#include <string>
#include <utility>
#include <vector>

#include "dictionary_segment.hpp"
#include "value_segment.hpp"

#include "resolve_type.hpp"
#include "types.hpp"
#include "utils/assert.hpp"

namespace opossum {

std::mutex chunks_mutex;
std::mutex compress_chunk_mutex;

Table::Table(const uint32_t chunk_size) {
  _max_chunk_size = chunk_size;
  _chunks = std::vector<std::shared_ptr<Chunk>>();
  _chunks.push_back(std::make_shared<Chunk>());
  _names = std::vector<std::string>();
  _types = std::vector<std::string>();
}

void Table::add_column(const std::string& name, const std::string& type) {
  _names.push_back(name);
  _types.push_back(type);
  if (_chunks.front()->size() == 0) {
    _chunks.back()->add_segment(make_shared_by_data_type<BaseSegment, ValueSegment>(type));
  } else {
    throw std::runtime_error("Error: Tried to add column to a non empty table");
  }
}

void Table::append(std::vector<AllTypeVariant> values) {
  // Locks _chunks to prevent adding multiple chunks
  chunks_mutex.lock();
  if (_chunks.back()->size() >= _max_chunk_size) {
    const std::shared_ptr<Chunk> new_chunk = std::make_shared<Chunk>();
    for (ColumnID column_id{0}; column_id < _names.size(); column_id++) {
      new_chunk->add_segment(make_shared_by_data_type<BaseSegment, ValueSegment>(_types[column_id]));
    }
    _chunks.push_back(std::move(new_chunk));
  }
  chunks_mutex.unlock();
  _chunks.back()->append(values);
}

uint16_t Table::column_count() const { return _names.size(); }

uint64_t Table::row_count() const {
  uint64_t count = 0;
  for (uint64_t i = 0; i < _chunks.size(); i++) {
    count += _chunks[i]->size();
  }
  return count;
}

ChunkID Table::chunk_count() const { return ChunkID{static_cast<uint32_t>(_chunks.size())}; }

ColumnID Table::column_id_by_name(const std::string& column_name) const {
  for (ColumnID column_id{0}; column_id < _names.size(); column_id++) {
    if (_names[column_id].compare(column_name) == 0) {
      return column_id;
    }
  }
  throw std::runtime_error("Column does not exist");
}

uint32_t Table::chunk_size() const { return _max_chunk_size; }

const std::vector<std::string>& Table::column_names() const { return _names; }

const std::string& Table::column_name(ColumnID column_id) const { return _names[column_id]; }

const std::string& Table::column_type(ColumnID column_id) const { return _types[column_id]; }

Chunk& Table::get_chunk(ChunkID chunk_id) { return *_chunks[chunk_id]; }

const Chunk& Table::get_chunk(ChunkID chunk_id) const { return *_chunks[chunk_id]; }

void Table::compress_chunk(ChunkID chunk_id) {
  auto& uncompressed_chunk = _chunks[chunk_id];
  DebugAssert(uncompressed_chunk->size() < _max_chunk_size, "Error: Chunk is not full");
  const auto compressed_chunk = std::make_shared<Chunk>();

  for (ColumnID column_id{0}; column_id < uncompressed_chunk->column_count(); column_id++) {
    const auto type = column_type(column_id);
    compressed_chunk->add_segment(
        make_shared_by_data_type<BaseSegment, DictionarySegment>(type, uncompressed_chunk->get_segment(column_id)));
  }

  compress_chunk_mutex.lock();
  uncompressed_chunk = compressed_chunk;
  compress_chunk_mutex.unlock();
}

}  // namespace opossum
