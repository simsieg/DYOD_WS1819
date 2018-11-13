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


Table::Table(const uint32_t chunk_size) {
  _max_chunk_size = chunk_size;
  _chunks.push_back(std::make_shared<Chunk>());
}

void Table::add_column(const std::string& name, const std::string& type) {
  DebugAssert(_chunks.front()->size() == 0, "Tried to add column to a non empty table");

  _column_names.push_back(name);
  _column_types.push_back(type);

  _chunks.back()->add_segment(make_shared_by_data_type<BaseSegment, ValueSegment>(type));
}

void Table::append(std::vector<AllTypeVariant> values) {
  std::lock_guard<std::mutex> lock(chunks_mutex);

  if (_chunks.back()->size() >= _max_chunk_size) {
    // latest chunk is full
    const auto new_chunk = std::make_shared<Chunk>();
    for(const auto& type : _column_types) {
      new_chunk->add_segment(
              make_shared_by_data_type<BaseSegment, ValueSegment>(type));
    }

    _chunks.push_back(std::move(new_chunk));
  }

  _chunks.back()->append(values);
}

uint16_t Table::column_count() const { return _column_names.size(); }

uint64_t Table::row_count() const {
  // not sure, whether this calculation uses 32 or 64 bit
  return (_chunks.size() - 1) * _max_chunk_size + _chunks.back()->size();

  // counting solution for later
//  uint64_t count = 0;
//  for (uint64_t i = 0; i < _chunks.size(); i++) {
//    count += _chunks[i]->size();
//  }
//  return count;
}

ChunkID Table::chunk_count() const { return ChunkID{static_cast<uint32_t>(_chunks.size())}; }

ColumnID Table::column_id_by_name(const std::string& column_name) const {
  auto const search_iter = std::find(
          _column_names.cbegin(),
          _column_names.cend(),
          column_name);
  DebugAssert( search_iter != _column_names.cend(), "Column does not exist");

  return ColumnID(std::distance(_column_names.cbegin(), search_iter));
}

uint32_t Table::chunk_size() const { return _max_chunk_size; }

const std::vector<std::string>& Table::column_names() const { return _column_names; }

const std::string& Table::column_name(ColumnID column_id) const { return _column_names[column_id]; }

const std::string& Table::column_type(ColumnID column_id) const { return _column_types[column_id]; }

Chunk& Table::get_chunk(ChunkID chunk_id) { return *_chunks[chunk_id]; }

const Chunk& Table::get_chunk(ChunkID chunk_id) const { return *_chunks[chunk_id]; }

void Table::compress_chunk(ChunkID chunk_id) {
  auto& uncompressed_chunk = _lock_chunk_for_compression(chunk_id);

  const auto compressed_chunk = std::make_shared<Chunk>();

  for (ColumnID column_id{0}; column_id < uncompressed_chunk->column_count(); column_id++) {
    const auto type = column_type(column_id);
    compressed_chunk->add_segment(
        make_shared_by_data_type<BaseSegment, DictionarySegment>(
                type,
                uncompressed_chunk->get_segment(column_id)));
  }

  // no mutex needed - operation is atomic
  uncompressed_chunk = compressed_chunk;
}

std::shared_ptr<Chunk>& Table::_lock_chunk_for_compression(ChunkID chunk_id) {
  static std::mutex _compression_start;
  std::lock_guard<std::mutex> compression_lock(_compression_start);

  auto& uncompressed_chunk = _chunks[chunk_id];

  DebugAssert( uncompressed_chunk->size() >= _max_chunk_size, "Chunk is not full");
  DebugAssert( ! uncompressed_chunk->compression_started(), "Chunk is already getting compressed");
  uncompressed_chunk->set_compression_start();

  return uncompressed_chunk;
}

}  // namespace opossum
