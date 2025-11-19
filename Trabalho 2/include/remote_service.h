// remote_service.h
#ifndef REMOTE_SERVICE_H
#define REMOTE_SERVICE_H

#include <vector>
#include <memory>
#include <string>

class Funcionario; 

class IRemoteSupermercadoService {
public:
    virtual ~IRemoteSupermercadoService() = default;
    
    // Métodos remotos
    virtual bool adicionarFuncionario(const std::vector<char>& funcionarioData) = 0;
    virtual std::vector<char> listarFuncionarios() = 0;
    virtual std::vector<char> buscarFuncionarioPorId(int id) = 0;
    virtual double calcularFolhaPagamento() = 0;
    virtual std::vector<char> getFuncionariosPorTipo(const std::string& tipo) = 0;
};

#endif