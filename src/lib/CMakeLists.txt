# Sources and libraries shared among the different builds of the lib
set(
    SOURCES
    all_type_variant.hpp
    resolve_type.hpp
    operators/abstract_operator.cpp
    operators/abstract_operator.hpp
    operators/get_table.hpp
    operators/print.cpp
    operators/print.hpp
    operators/table_scan.hpp
    operators/table_wrapper.cpp
    operators/table_wrapper.hpp
    storage/base_attribute_vector.hpp
    storage/base_segment.hpp
    storage/chunk.cpp
    storage/chunk.hpp
    storage/dictionary_segment.hpp
    storage/reference_segment.hpp
    storage/storage_manager.cpp
    storage/storage_manager.hpp
    storage/table.cpp
    storage/table.hpp
    storage/value_segment.cpp
    storage/value_segment.hpp
    type_cast.cpp
    type_cast.hpp
    types.hpp
    utils/assert.hpp
    utils/load_table.cpp
    utils/load_table.hpp
)

set(
    LIBRARIES
    pthread
)

# Configure the regular hyrise library used for tests/server/playground...
add_library(hyrise STATIC ${SOURCES})
target_link_libraries(hyrise ${LIBRARIES})

# Configure the lib used for sanitizers
add_library(hyriseSanitizersLib EXCLUDE_FROM_ALL STATIC ${SOURCES})
target_link_libraries(
    hyriseSanitizersLib
    ${LIBRARIES}
    -fsanitize=address,undefined -fno-sanitize-recover=all
)
set_target_properties(hyriseSanitizersLib PROPERTIES COMPILE_FLAGS "-fsanitize=address,undefined -fno-sanitize-recover=all -fno-omit-frame-pointer")

# Configure the lib used for coverage
add_library(hyriseCoverageLib EXCLUDE_FROM_ALL STATIC ${SOURCES})
target_link_libraries(
    hyriseCoverageLib
    ${LIBRARIES}
    --coverage
)
set_target_properties(hyriseCoverageLib PROPERTIES COMPILE_FLAGS "-fprofile-arcs -ftest-coverage")
