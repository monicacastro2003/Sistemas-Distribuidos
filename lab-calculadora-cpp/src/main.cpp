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

// HTML SIMPLES sem JavaScript complexo
std::string get_html_page() {
    return R"(<!DOCTYPE html>
<html>
<head>
    <title>Calculadora C++</title>
    <style>
        body { font-family: Arial; margin: 40px; }
        .container { max-width: 400px; margin: auto; }
        input, button { margin: 5px; padding: 10px; width: 100%; }
        button { background: #4CAF50; color: white; border: none; }
    </style>
</head>
<body>
    <div class="container">
        <h2>Calculadora Web (C++)</h2>
        <p>Teste os endpoints:</p>
        <ul>
            <li><a href="/somar/10/5">/somar/10/5</a></li>
            <li><a href="/subtrair/10/5">/subtrair/10/5</a></li>
            <li><a href="/multiplicar/10/5">/multiplicar/10/5</a></li>
            <li><a href="/dividir/10/5">/dividir/10/5</a></li>
        </ul>
        <p>Ou use diretamente no navegador:</p>
        <p><code>http://localhost:8080/somar/15/3</code></p>
    </div>
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
    
    // Roteamento
    if (target == "/" || target == "/index.html") {
        res.result(http::status::ok);
        res.body() = get_html_page();
    } 
    else if (target.find("/somar/") == 0) {
        try {
            size_t pos1 = 7;
            size_t pos2 = target.find('/', pos1);
            if (pos2 != std::string::npos) {
                double a = std::stod(target.substr(pos1, pos2 - pos1));
                double b = std::stod(target.substr(pos2 + 1));
                res.result(http::status::ok);
                res.body() = calcular("somar", a, b);
            } else {
                throw std::invalid_argument("Formato invalido");
            }
        } catch (...) {
            res.result(http::status::bad_request);
            res.body() = "Use: /somar/10/5";
        }
    }
    else if (target.find("/subtrair/") == 0) {
        try {
            size_t pos1 = 10;
            size_t pos2 = target.find('/', pos1);
            if (pos2 != std::string::npos) {
                double a = std::stod(target.substr(pos1, pos2 - pos1));
                double b = std::stod(target.substr(pos2 + 1));
                res.result(http::status::ok);
                res.body() = calcular("subtrair", a, b);
            } else {
                throw std::invalid_argument("Formato invalido");
            }
        } catch (...) {
            res.result(http::status::bad_request);
            res.body() = "Use: /subtrair/10/5";
        }
    }
    else if (target.find("/multiplicar/") == 0) {
        try {
            size_t pos1 = 13;
            size_t pos2 = target.find('/', pos1);
            if (pos2 != std::string::npos) {
                double a = std::stod(target.substr(pos1, pos2 - pos1));
                double b = std::stod(target.substr(pos2 + 1));
                res.result(http::status::ok);
                res.body() = calcular("multiplicar", a, b);
            } else {
                throw std::invalid_argument("Formato invalido");
            }
        } catch (...) {
            res.result(http::status::bad_request);
            res.body() = "Use: /multiplicar/10/5";
        }
    }
    else if (target.find("/dividir/") == 0) {
        try {
            size_t pos1 = 9;
            size_t pos2 = target.find('/', pos1);
            if (pos2 != std::string::npos) {
                double a = std::stod(target.substr(pos1, pos2 - pos1));
                double b = std::stod(target.substr(pos2 + 1));
                res.result(http::status::ok);
                res.body() = calcular("dividir", a, b);
            } else {
                throw std::invalid_argument("Formato invalido");
            }
        } catch (...) {
            res.result(http::status::bad_request);
            res.body() = "Use: /dividir/10/5";
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
        std::cout << "  GET /somar/{num1}/{num2}" << std::endl;
        std::cout << "  GET /subtrair/{num1}/{num2}" << std::endl;
        std::cout << "  GET /multiplicar/{num1}/{num2}" << std::endl;
        std::cout << "  GET /dividir/{num1}/{num2}" << std::endl;
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
