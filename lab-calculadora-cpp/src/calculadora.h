#ifndef CALCULADORA_H
#define CALCULADORA_H

#include <string>

class Calculadora {
public:
    static std::string somar(double a, double b);
    static std::string subtrair(double a, double b);
    static std::string multiplicar(double a, double b);
    static std::string dividir(double a, double b);
    
    static bool isNumber(const std::string& str);
    static double parseDouble(const std::string& str);
};

#endif
