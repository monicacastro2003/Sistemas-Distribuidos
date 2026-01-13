#include "serializer.h"
#include "subclasses.h"
#include <sstream>
#include <regex>
#include <iostream>

std::string Serializer::funcionarioToJson(const std::shared_ptr<Funcionario>& funcionario) {
    if (!funcionario) return "{}";
    
    std::ostringstream json;
    json << "{";
    json << "\"id\":" << funcionario->getId() << ",";
    json << "\"nome\":\"" << funcionario->getNome() << "\",";
    json << "\"salario\":" << funcionario->getSalario() << ",";
    json << "\"tipo\":\"" << funcionario->tipo() << "\",";
    json << "\"escala\":\"" << funcionario->mostrarEscala() << "\"";
    
    // Adiciona campos específicos
    std::string tipo = funcionario->tipo();
    if (tipo == "Vendedor") {
        auto vendedor = std::dynamic_pointer_cast<Vendedor>(funcionario);
        if (vendedor) {
            json << ",\"comissao\":" << vendedor->getComissao();
        }
    } else if (tipo == "Gerente") {
        auto gerente = std::dynamic_pointer_cast<Gerente>(funcionario);
        if (gerente) {
            json << ",\"bonus\":" << gerente->getBonus();
        }
    } else if (tipo == "Caixa") {
        auto caixa = std::dynamic_pointer_cast<Caixa>(funcionario);
        if (caixa) {
            json << ",\"numeroCaixa\":" << caixa->getNumeroCaixa();
        }
    } else if (tipo == "Balconista") {
        auto balconista = std::dynamic_pointer_cast<Balconista>(funcionario);
        if (balconista) {
            json << ",\"setor\":\"" << balconista->getSetor() << "\"";
        }
    }
    
    json << "}";
    return json.str();
}

std::shared_ptr<Funcionario> Serializer::jsonToFuncionario(const std::string& jsonStr) {
    // Extrai valores do JSON simples (implementação básica)
    // Em produção, usar uma biblioteca JSON como nlohmann/json
    
    std::regex id_regex("\"id\":(\\d+)");
    std::regex nome_regex("\"nome\":\"([^\"]+)\"");
    std::regex salario_regex("\"salario\":([0-9.]+)");
    std::regex tipo_regex("\"tipo\":\"([^\"]+)\"");
    std::regex comissao_regex("\"comissao\":([0-9.]+)");
    std::regex bonus_regex("\"bonus\":([0-9.]+)");
    std::regex numcaixa_regex("\"numeroCaixa\":(\\d+)");
    std::regex setor_regex("\"setor\":\"([^\"]+)\"");
    
    std::smatch matches;
    
    // Extrai tipo primeiro
    std::string tipo;
    if (std::regex_search(jsonStr, matches, tipo_regex) && matches.size() > 1) {
        tipo = matches[1].str();
    }
    
    if (tipo.empty()) return nullptr;
    
    // Extrai dados básicos
    int id = 0;
    std::string nome;
    double salario = 0.0;
    
    if (std::regex_search(jsonStr, matches, id_regex) && matches.size() > 1) {
        id = std::stoi(matches[1].str());
    }
    
    if (std::regex_search(jsonStr, matches, nome_regex) && matches.size() > 1) {
        nome = matches[1].str();
    }
    
    if (std::regex_search(jsonStr, matches, salario_regex) && matches.size() > 1) {
        salario = std::stod(matches[1].str());
    }
    
    // Cria o funcionário baseado no tipo
    std::shared_ptr<Funcionario> funcionario;
    
    if (tipo == "Vendedor") {
        double comissao = 0.0;
        if (std::regex_search(jsonStr, matches, comissao_regex) && matches.size() > 1) {
            comissao = std::stod(matches[1].str());
        }
        funcionario = std::make_shared<Vendedor>(id, nome, salario, comissao);
    }
    else if (tipo == "Gerente") {
        double bonus = 0.0;
        if (std::regex_search(jsonStr, matches, bonus_regex) && matches.size() > 1) {
            bonus = std::stod(matches[1].str());
        }
        funcionario = std::make_shared<Gerente>(id, nome, salario, bonus);
    }
    else if (tipo == "Caixa") {
        int numeroCaixa = 0;
        if (std::regex_search(jsonStr, matches, numcaixa_regex) && matches.size() > 1) {
            numeroCaixa = std::stoi(matches[1].str());
        }
        funcionario = std::make_shared<Caixa>(id, nome, salario, numeroCaixa);
    }
    else if (tipo == "Balconista") {
        std::string setor;
        if (std::regex_search(jsonStr, matches, setor_regex) && matches.size() > 1) {
            setor = matches[1].str();
        }
        funcionario = std::make_shared<Balconista>(id, nome, salario, setor);
    }
    
    return funcionario;
}

std::string Serializer::funcionariosToJson(const std::vector<std::shared_ptr<Funcionario>>& funcionarios) {
    std::ostringstream json;
    json << "[";
    
    for (size_t i = 0; i < funcionarios.size(); ++i) {
        json << funcionarioToJson(funcionarios[i]);
        if (i < funcionarios.size() - 1) {
            json << ",";
        }
    }
    
    json << "]";
    return json.str();
}

std::string Serializer::supermercadoToJson(const Supermercado& supermercado) {
    std::ostringstream json;
    json << "{";
    json << "\"nome\":\"" << "MeuSuper" << "\",";
    json << "\"funcionarios\":" << funcionariosToJson(supermercado.getFuncionarios());
    json << "}";
    return json.str();
}

std::string Serializer::createResponse(bool success, const std::string& message, const std::string& data) {
    std::ostringstream json;
    json << "{";
    json << "\"success\":" << (success ? "true" : "false") << ",";
    json << "\"message\":\"" << message << "\"";
    
    if (!data.empty()) {
        json << ",\"data\":" << data;
    }
    
    json << "}";
    return json.str();
}

std::string Serializer::createError(const std::string& message) {
    return createResponse(false, message);
}