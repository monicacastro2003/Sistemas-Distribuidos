cmake_minimum_required(VERSION 3.10)
project(CalculadoraServer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Encontrar Boost
find_package(Boost REQUIRED COMPONENTS system)

if(Boost_FOUND)
    message(STATUS "Boost encontrado: ${Boost_VERSION}")
    include_directories(${Boost_INCLUDE_DIRS})
else()
    message(FATAL_ERROR "Boost não encontrado! Instale com: sudo apt install libboost-all-dev")
endif()

# Arquivos fonte
set(SOURCE_FILES
    src/main.cpp
    src/calculadora.cpp
)

# Executável principal
add_executable(calculadora_server ${SOURCE_FILES})

# Linkar bibliotecas Boost
target_link_libraries(calculadora_server
    ${Boost_LIBRARIES}
)

# Mensagem de ajuda
message("\n=== CONFIGURACAO CONCLUIDA ===")
message("Para compilar:")
message("  mkdir build && cd build")
message("  cmake ..")
message("  make")
message("")
message("Para executar:")
message("  ./calculadora_server")
message("=== ======================= ===")
