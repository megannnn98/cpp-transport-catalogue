cmake_minimum_required(VERSION 3.18)

project(search_server)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(search_server    main.cpp 
                                input_reader.cpp
                                stat_reader.cpp
                                transport_catalogue.cpp)

set_target_properties(search_server PROPERTIES CXX_STANDARD 17)

add_compile_options(-Wall 
                    -Wextra 
                    -pedantic 
                    -Werror
                    -Wmisleading-indentation
                    -Wduplicated-cond
                    -Wduplicated-branches
                    -Wlogical-op
                    -Wuseless-cast
                    )
