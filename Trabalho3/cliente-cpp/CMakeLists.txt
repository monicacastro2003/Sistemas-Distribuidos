cmake_minimum_required(VERSION 3.10)
project(ClienteSupermercadoHTTP)

set(CMAKE_CXX_STANDARD 17)

# Encontrar libcurl
find_package(CURL REQUIRED)

# Baixar nlohmann/json
include(FetchContent)
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.2
)
FetchContent_MakeAvailable(nlohmann_json)

add_executable(cliente cliente.cpp)

target_link_libraries(cliente
    ${CURL_LIBRARIES}
    nlohmann_json::nlohmann_json
)

message("\n==================================================")
message("CLIENTE HTTP C++ - SUPERMERCADO API")
message("==================================================")
message("Transformação do cliente RMI para HTTP")
message("")
message("Para compilar:")
message("  mkdir build && cd build")
message("  cmake .. && make")
message("")
message("Para executar (primeiro inicie o servidor):")
message("  ./cliente_http")
message("==================================================\n")