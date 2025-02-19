project(tileson)

set(CMAKE_CXX_STANDARD 20)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE include)

set_target_properties(tileson PROPERTIES INTERFACE_SYSTEM_INCLUDE_DIRECTORIES $<TARGET_PROPERTY:tileson,INTERFACE_INCLUDE_DIRECTORIES>)
