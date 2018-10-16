# Configure playground
add_executable(
    hyrisePlayground

    playground.cpp
)
target_link_libraries(
    hyrisePlayground
    hyrise
)
