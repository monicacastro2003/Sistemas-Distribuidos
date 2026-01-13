#ifndef SERIALIZER_H
#define SERIALIZER_H

#include "funcionario.h"
#include "supermercado.h"
#include <string>
#include <vector>
#include <memory>

class Serializer {
public:
    // Serializa um funcionário para JSON
    static std::string funcionarioToJson(const std::shared_ptr<Funcionario>& funcionario);
    
    // Desserializa JSON para funcionário
    static std::shared_ptr<Funcionario> jsonToFuncionario(const std::string& jsonStr);
    
    // Serializa lista de funcionários para JSON
    static std::string funcionariosToJson(const std::vector<std::shared_ptr<Funcionario>>& funcionarios);
    
    // Serializa Supermercado para JSON
    static std::string supermercadoToJson(const Supermercado& supermercado);
    
    // Cria JSON de resposta padrão
    static std::string createResponse(bool success, const std::string& message, const std::string& data = "");
    
    // Cria JSON de erro
    static std::string createError(const std::string& message);
};

#endif