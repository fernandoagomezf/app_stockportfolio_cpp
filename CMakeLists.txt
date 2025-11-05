cmake_minimum_required(VERSION 4.1.0)

project(spt
    VERSION 0.1.0
    DESCRIPTION "Stock Portfolio Monitor"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
set(wxWidgets_USE_STATIC ON)

find_package(SQLite3 REQUIRED)
find_package(wxWidgets REQUIRED COMPONENTS core base OPTIONAL_COMPONENTS net gl)

include(${wxWidgets_USE_FILE})

add_executable(spt WIN32
    # domain module
    src/spt.domain/spt.domain.ixx
    # infrastructure module
    src/spt.infrastructure.sql/spt.infrastructure.sql.ixx
    src/spt.infrastructure.sql/value.ixx
    src/spt.infrastructure.sql/value.cpp
    src/spt.infrastructure.sql/row.ixx
    src/spt.infrastructure.sql/row.cpp
    src/spt.infrastructure.sql/resultset.ixx
    src/spt.infrastructure.sql/resultset.cpp
    src/spt.infrastructure.sql/statement.ixx
    src/spt.infrastructure.sql/statement.cpp
    src/spt.infrastructure.sql/database.ixx
    src/spt.infrastructure.sql/database.cpp
    src/spt.infrastructure.repositories/spt.infrastructure.repositories.ixx
    src/spt.infrastructure.repositories/repository.ixx
    src/spt.infrastructure.repositories/repository.cpp
    # application module
    src/spt.app/application.ixx
    src/spt.app/application.cpp
    src/spt.app/window.ixx
    src/spt.app/window.cpp
)
target_link_libraries(spt PRIVATE SQLite::SQLite3)
target_link_libraries(spt PRIVATE ${wxWidgets_LIBRARIES})
target_compile_features(spt PUBLIC cxx_std_23)