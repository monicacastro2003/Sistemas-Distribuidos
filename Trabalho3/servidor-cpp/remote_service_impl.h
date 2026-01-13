#ifndef REMOTE_SERVICE_IMPL_H
#define REMOTE_SERVICE_IMPL_H

#include "remote_service.h"
#include "supermercado.h"
#include "serializer.h"
#include <vector>
#include <memory>
#include <sstream>

// Declarações das funções do serializer que vamos usar
std::vector<char> packFuncionario(const Funcionario& f);
std::shared_ptr<Funcionario> unpackFuncionario(const std::vector<char>& buf);

class RemoteSupermercadoService : public IRemoteSupermercadoService {
private:
    Supermercado supermercado;
    
public:
    RemoteSupermercadoService(const std::string& nome = "Supermercado Central") 
        : supermercado(nome) {}
    
    bool adicionarFuncionario(const std::vector<char>& funcionarioData) override {
        try {
            auto funcionario = unpackFuncionario(funcionarioData);
            supermercado.adicionar(funcionario);
            return true;
        } catch (...) {
            return false;
        }
    }
    
    std::vector<char> listarFuncionarios() override {
        auto funcionarios = supermercado.getFuncionarios();
        return serializeFuncionarioList(funcionarios);
    }
    
    std::vector<char> buscarFuncionarioPorId(int id) override {
        auto funcionarios = supermercado.getFuncionarios();
        for (const auto& func : funcionarios) {
            if (func->getId() == id) {
                return packFuncionario(*func);
            }
        }
        return std::vector<char>();
    }
    
    double calcularFolhaPagamento() override {
        double total = 0.0;
        auto funcionarios = supermercado.getFuncionarios();
        for (const auto& func : funcionarios) {
            total += func->getSalario();
        }
        return total;
    }
    
    std::vector<char> getFuncionariosPorTipo(const std::string& tipo) override {
        std::vector<std::shared_ptr<Funcionario>> resultado;
        auto funcionarios = supermercado.getFuncionarios();
        
        for (const auto& func : funcionarios) {
            if (func->tipo() == tipo) {
                resultado.push_back(func);
            }
        }
        
        return serializeFuncionarioList(resultado);
    }
    
private:
    std::vector<char> serializeFuncionarioList(const std::vector<std::shared_ptr<Funcionario>>& lista) {
        std::ostringstream os(std::ios::binary);
        
        // Serializa número de elementos
        int32_t n = (int32_t)lista.size();
        os.write(reinterpret_cast<const char*>(&n), sizeof(n));
        
        // Serializa cada funcionário
        for (const auto &fptr : lista) {
            auto buf = packFuncionario(*fptr);
            int32_t bytes = (int32_t)buf.size();
            os.write(reinterpret_cast<const char*>(&bytes), sizeof(bytes));
            os.write(buf.data(), buf.size());
        }
        
        std::string str = os.str();
        return std::vector<char>(str.begin(), str.end());
    }
};

#endif