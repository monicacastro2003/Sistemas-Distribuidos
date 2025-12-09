#include "calculadora.h"
#include <sstream>
#include <iomanip>
#include <cctype>

std::string Calculadora::somar(double a, double b) {
    double resultado = a + b;
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << "Resultado: " << resultado;
    return oss.str();
}

std::string Calculadora::subtrair(double a, double b) {
    double resultado = a - b;
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << "Resultado: " << resultado;
    return oss.str();
}

std::string Calculadora::multiplicar(double a, double b) {
    double resultado = a * b;
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << "Resultado: " << resultado;
    return oss.str();
}

std::string Calculadora::dividir(double a, double b) {
    if (b == 0) {
        return "Erro: Divisao por zero!";
    }
    double resultado = a / b;
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << "Resultado: " << resultado;
    return oss.str();
}

bool Calculadora::isNumber(const std::string& str) {
    if (str.empty()) return false;
    
    bool hasDecimal = false;
    bool hasDigit = false;
    
    for (size_t i = 0; i < str.length(); ++i) {
        char c = str[i];
        
        if (i == 0 && (c == '+' || c == '-')) {
            continue; // Sinal permitido no início
        }
        
        if (c == '.') {
            if (hasDecimal) return false; // Só um ponto decimal
            hasDecimal = true;
        } else if (std::isdigit(c)) {
            hasDigit = true;
        } else {
            return false;
        }
    }
    
    return hasDigit;
}

double Calculadora::parseDouble(const std::string& str) {
    return std::stod(str);
}
