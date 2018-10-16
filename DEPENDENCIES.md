# Dependencies

| Name             | Version       | Platform |                Optional |
| ---------------- | ------------- | -------- | ----------------------- |
| build-essential  | any           |    Linux |                      No |
| boost            | >= 1.63.0     |    All   |                      No |
| clang            | >= 4          |    All   |   Yes, if gcc installed |
| clang-format     | 3.8           |    All   |        Yes (formatting) |
| cmake            | 3.5           |    All   |                      No |
| gcc              | 7.2           |    All   | Yes, if clang installed |
| gcovr            | >= 3.2        |    All   |          Yes (coverage) |
| llvm             | any           |    All   |   Yes (code sanitizers) |
| parallel         | any           |    All   |                     Yes |
| python           | >= 2.7 && < 3 |    All   |           Yes (linting) |


## Dependencies that are integrated in our build process via git submodules
- googletest (https://github.com/google/googletest)
