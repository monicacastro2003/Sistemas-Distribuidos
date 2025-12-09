#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

// Função da calculadora
std::string calcular(const std::string& operacao, double a, double b) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);
    
    if (operacao == "somar") {
        oss << "Resultado: " << (a + b);
    } else if (operacao == "subtrair") {
        oss << "Resultado: " << (a - b);
    } else if (operacao == "multiplicar") {
        oss << "Resultado: " << (a * b);
    } else if (operacao == "dividir") {
        if (b == 0) {
            return "Erro: Divisao por zero!";
        }
        oss << "Resultado: " << (a / b);
    } else {
        return "Operacao invalida";
    }
    
    return oss.str();
}

// HTML com escapes CORRETOS
std::string get_html_page() {
    // Note: usamos \\' para escapar aspas simples dentro da string raw
    return R"(<!DOCTYPE html>
<html>
<head>
    <title>Calculadora C++</title>
    <style>
        body { font-family: Arial; margin: 40px; }
        input, button { margin: 5px; padding: 10px; }
        button { background: #4CAF50; color: white; border: none; }
    </style>
</head>
<body>
    <h2>Calculadora Web (C++)</h2>
    <input type="number" id="num1" placeholder="Numero 1">
    <input type="number" id="num2" placeholder="Numero 2">
    <button onclick="calcular('somar')">Somar</button>
    <button onclick="calcular('subtrair')">Subtrair</button>
    <button onclick="calcular('multiplicar')">Multiplicar</button>
    <button onclick="calcular('dividir')">Dividir</button>
    <p id="resultado"></p>
    
    <script>
        function calcular(op) {
            const num1 = document.getElementById(\"num1\").value;
            const num2 = document.getElementById(\"num2\").value;
            fetch('http://localhost:8080/' + op + '/' + num1 + '/' + num2)
                .then(r => r.text())
                .then(data => document.getElementById(\"resultado\").innerText = data);
        }
    </script>
</body>
</html>)";
}

void handle_request(http::request<http::string_body>& req, http::response<http::string_body>& res) {
    res.version(req.version());
    res.set(http::field::server, "Calculadora C++ Server");
    res.set(http::field::content_type, "text/html");
    res.set(http::field::access_control_allow_origin, "*");
    
    std::string target = req.target();
    std::cout << "Requisicao: " << req.method_string() << " " << target << std::endl;
    
    // Roteamento básico
    if (target == "/" || target == "/index.html") {
        res.result(http::status::ok);
        res.body() = get_html_page();
    } 
    else if (target.find("/somar/") == 0) {
        size_t pos1 = 7;
        size_t pos2 = target.find('/', pos1);
        
        if (pos2 != std::string::npos) {
            try {
                double a = std::stod(target.substr(pos1, pos2 - pos1));
                double b = std::stod(target.substr(pos2 + 1));
                res.result(http::status::ok);
                res.body() = calcular("somar", a, b);
            } catch (...) {
                res.result(http::status::bad_request);
                res.body() = "Numeros invalidos";
            }
        } else {
            res.result(http::status::bad_request);
            res.body() = "Formato: /somar/num1/num2";
        }
    }
    else if (target.find("/subtrair/") == 0) {
        size_t pos1 = 10;
        size_t pos2 = target.find('/', pos1);
        
        if (pos2 != std::string::npos) {
            try {
                double a = std::stod(target.substr(pos1, pos2 - pos1));
                double b = std::stod(target.substr(pos2 + 1));
                res.result(http::status::ok);
                res.body() = calcular("subtrair", a, b);
            } catch (...) {
                res.result(http::status::bad_request);
                res.body() = "Numeros invalidos";
            }
        } else {
            res.result(http::status::bad_request);
            res.body() = "Formato: /subtrair/num1/num2";
        }
    }
    else if (target.find("/multiplicar/") == 0) {
        size_t pos1 = 13;
        size_t pos2 = target.find('/', pos1);
        
        if (pos2 != std::string::npos) {
            try {
                double a = std::stod(target.substr(pos1, pos2 - pos1));
                double b = std::stod(target.substr(pos2 + 1));
                res.result(http::status::ok);
                res.body() = calcular("multiplicar", a, b);
            } catch (...) {
                res.result(http::status::bad_request);
                res.body() = "Numeros invalidos";
            }
        } else {
            res.result(http::status::bad_request);
            res.body() = "Formato: /multiplicar/num1/num2";
        }
    }
    else if (target.find("/dividir/") == 0) {
        size_t pos1 = 9;
        size_t pos2 = target.find('/', pos1);
        
        if (pos2 != std::string::npos) {
            try {
                double a = std::stod(target.substr(pos1, pos2 - pos1));
                double b = std::stod(target.substr(pos2 + 1));
                res.result(http::status::ok);
                res.body() = calcular("dividir", a, b);
            } catch (...) {
                res.result(http::status::bad_request);
                res.body() = "Numeros invalidos";
            }
        } else {
            res.result(http::status::bad_request);
            res.body() = "Formato: /dividir/num1/num2";
        }
    }
    else {
        res.result(http::status::not_found);
        res.body() = "404 - Endpoint nao encontrado";
    }
    
    res.prepare_payload();
}

int main() {
    try {
        unsigned short port = 8080;
        net::io_context ioc;
        
        tcp::acceptor acceptor(ioc, tcp::endpoint(tcp::v4(), port));
        
        std::cout << "=====================================" << std::endl;
        std::cout << "Servidor Calculadora C++ iniciado!" << std::endl;
        std::cout << "Acesse: http://localhost:" << port << "/" << std::endl;
        std::cout << "Endpoints disponiveis:" << std::endl;
        std::cout << "  /somar/{num1}/{num2}" << std::endl;
        std::cout << "  /subtrair/{num1}/{num2}" << std::endl;
        std::cout << "  /multiplicar/{num1}/{num2}" << std::endl;
        std::cout << "  /dividir/{num1}/{num2}" << std::endl;
        std::cout << "=====================================" << std::endl;
        
        while (true) {
            tcp::socket socket(ioc);
            acceptor.accept(socket);
            
            beast::flat_buffer buffer;
            http::request<http::string_body> req;
            http::read(socket, buffer, req);
            
            http::response<http::string_body> res;
            handle_request(req, res);
            
            http::write(socket, res);
            socket.shutdown(tcp::socket::shutdown_send);
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Erro: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
