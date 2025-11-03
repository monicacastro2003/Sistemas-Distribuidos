# ============================
# Makefile - Trabalho SD
# Gestão de Supermercado
# ============================

CXX = g++
CXXFLAGS = -std=c++17 -Iinclude -Wall -Wextra -Wno-unused-function -pthread

# Arquivos-fonte
SERVER_SRC = src/servidor.cpp src/serializer.cpp
CLIENT_SRC = src/cliente.cpp src/serializer.cpp

# Alvos
SERVER_BIN = servidor
CLIENT_BIN = cliente

# ============================
# Alvo padrão: compila tudo
# ============================
all: $(SERVER_BIN) $(CLIENT_BIN)

$(SERVER_BIN): $(SERVER_SRC)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(CLIENT_BIN): $(CLIENT_SRC)
	$(CXX) $(CXXFLAGS) $^ -o $@

# ============================
# Limpar arquivos gerados
# ============================
clean:
	rm -f $(SERVER_BIN) $(CLIENT_BIN) funcs.bin

# ============================
# Executar normalmente
# ============================
run-server:
	./servidor

run-client:
	./cliente
