project(SandBox_Runner)

set(CMAKE_CXX_STANDARD 11)

add_executable(SandBox_Runner main.cpp common.h configureparser.cpp configureparser.h sandboxer.cpp sandboxer.h)
