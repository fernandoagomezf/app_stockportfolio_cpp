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

find_package(Boost REQUIRED COMPONENTS uuid)
find_package(SQLite3 REQUIRED)
find_package(CURL REQUIRED)
find_package(wxWidgets REQUIRED COMPONENTS core base OPTIONAL_COMPONENTS net gl)

include(${wxWidgets_USE_FILE})

add_executable(spt WIN32
    # domain module
    src/spt.domain/spt.domain.ixx
    src/spt.domain/pricepoint.cpp
    # infrastructure module
    src/spt.infrastructure/spt.infrastructure.ixx
    src/spt.infrastructure/value.cpp
    src/spt.infrastructure/row.cpp
    src/spt.infrastructure/resultset.cpp    
    src/spt.infrastructure/statement.cpp    
    src/spt.infrastructure/database.cpp    
    src/spt.infrastructure/httpheaders.cpp
    src/spt.infrastructure/httprequest.cpp
    src/spt.infrastructure/httpresponse.cpp
    src/spt.infrastructure/httpclient.cpp
    src/spt.infrastructure/jsonvalue.cpp    
    src/spt.infrastructure/jsonparser.cpp    
    src/spt.infrastructure/repository.cpp
    # application module
    src/spt.app/spt.app.ixx
    src/spt.app/application.cpp
    src/spt.app/window.cpp
)
target_link_libraries(spt PRIVATE Boost::uuid)
target_link_libraries(spt PRIVATE SQLite::SQLite3)
target_link_libraries(spt PRIVATE ${wxWidgets_LIBRARIES})
target_link_libraries(spt PRIVATE CURL::libcurl)
target_compile_features(spt PUBLIC cxx_std_23)
