LABORATÓRIO: Cliente PHP para WebService Spring Boot

Disciplina: Sistemas Distribuídos
Professor: Rafael Braga
Alunos: Mônica Maria Rodrigues De Castro e Maryana Moraes Sousa
Data: Dezembro 2025

DESCRIÇÃO
Sistema distribuído com API Spring Boot consumida por cliente PHP via HTTP.

ESTRUTURA
text
lab_ws_PHP/
├── demo/                    # API Spring Boot Java
│   ├── src/main/java/com/demo/Main.java
│   └── pom.xml
└── cliente-php/            # Cliente PHP
    ├── index.php
    ├── calcular.php
    └── resultado.php
EXECUTAR
bash
# Terminal 1 - Spring Boot
cd lab_ws_PHP/demo
mvn spring-boot:run
# API: http://localhost:8080/

# Terminal 2 - PHP
cd lab_ws_PHP/cliente-php
php -S localhost:8000
# Cliente: http://localhost:8000/
ENDPOINTS
GET /somar/{num1}/{num2}

GET /subtrair/{num1}/{num2}

GET /multiplicar/{num1}/{num2}

GET /dividir/{num1}/{num2}

TESTES
bash
curl http://localhost:8080/somar/10/5      # 15.00
curl http://localhost:8080/dividir/10/0    # ERRO: Divisao por zero
TECNOLOGIAS
Java 11, Spring Boot, Maven

PHP 8.3, HTML, cURL

HTTP/REST
