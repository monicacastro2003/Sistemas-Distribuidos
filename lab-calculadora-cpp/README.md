# Laboratório: Web Services com C++

## Descrição
Implementação de um servidor HTTP em C++ usando Boost.Beast que fornece uma API REST para operações matemáticas básicas.

## Tecnologias
- Backend: C++17, Boost.Beast, Boost.Asio
- Frontend: HTML5, CSS3, JavaScript
- Build: CMake, Make
- Protocolo: HTTP/1.1

## Como Executar

### 1. Instalar dependências
sudo apt update
sudo apt install g++ cmake libboost-all-dev -y

### 2. Compilar
mkdir build && cd build
cmake ..
make

### 3. Executar
./calculadora_server

### 4. Testar
# Teste a API
curl http://localhost:8080/somar/10/5

# Acesse a interface web
# http://localhost:8080/

## Endpoints da API
- GET /somar/{a}/{b} - Soma dois números
- GET /subtrair/{a}/{b} - Subtrai dois números
- GET /multiplicar/{a}/{b} - Multiplica dois números
- GET /dividir/{a}/{b} - Divide dois números (trata divisão por zero)

## Estrutura do Projeto
.
├── CMakeLists.txt          # Configuração de build
├── README.md              # Este arquivo
├── src/                   # Código C++
│   ├── main.cpp          # Servidor HTTP
│   ├── calculadora.cpp   # Lógica da calculadora
│   └── calculadora.h     # Cabeçalho
└── public/               # Interface web
    └── calculadora.html  # Página HTML

## Equipe
- Mônica Maria Rodrigues de Castro
- Maryana Moraes Sousa

## Disciplina
Sistemas Distribuídos (QXD0043) - UFC Campus Quixadá
Professor: Rafael Braga
