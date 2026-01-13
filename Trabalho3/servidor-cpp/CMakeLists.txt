cmake_minimum_required(VERSION 3.10)
project(ServidorSupermercadoHTTP)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

# Listar arquivos
set(SOURCE_FILES
    servidor_http.cpp
    funcionario.cpp
    subclasses.cpp
    serializer.cpp
)

add_executable(servidor_http ${SOURCE_FILES})
target_link_libraries(servidor_http pthread)

# Incluir diretório atual
target_include_directories(servidor_http PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

message("\n=== SERVIDOR SUPERMERCADO API ===")
message("Compile: mkdir build && cd build && cmake .. && make")
message("Execute: ./servidor_http")
message("URL: http://localhost:8080")