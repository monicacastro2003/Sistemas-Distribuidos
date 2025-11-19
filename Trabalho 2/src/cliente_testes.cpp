#include "../include/rmi_core.h"
#include "../include/subclasses.h"
#include "../include/serializer.h"
#include <iostream>
#include <cstring>
#include <iomanip>
#include <sstream>  // ADICIONADO

// Função para serializar um funcionário para envio
std::vector<char> serializarFuncionarioParaEnvio(const Funcionario& func) {
    return packFuncionario(func);
}

// Função para desserializar double corretamente
double deserializeDouble(const std::vector<char>& data) {
    if (data.size() != sizeof(double)) {
        std::cout << "AVISO: Tamanho incorreto para double" << std::endl;
        return 0.0;
    }
    double value;
    memcpy(&value, data.data(), sizeof(double));
    return value;
}

// Função para desserializar bool
bool deserializeBool(const std::vector<char>& data) {
    return !data.empty() && data[0] != 0;
}

// FUNÇÃO NOVA: Desserializar lista de funcionários
std::vector<std::shared_ptr<Funcionario>> unpackFuncionarioList(const std::vector<char>& data) {
    std::vector<std::shared_ptr<Funcionario>> result;
    
    if (data.empty()) {
        return result;
    }
    
    try {
        std::string str(data.begin(), data.end());
        std::istringstream is(str, std::ios::binary);
        
        // Lê o número de elementos
        int32_t n;
        is.read(reinterpret_cast<char*>(&n), sizeof(n));
        
        std::cout << "Desserializando " << n << " funcionários..." << std::endl;
        
        // Lê cada funcionário
        for (int i = 0; i < n; ++i) {
            int32_t bytes;
            is.read(reinterpret_cast<char*>(&bytes), sizeof(bytes));
            std::vector<char> buf(bytes);
            is.read(buf.data(), bytes);
            result.push_back(unpackFuncionario(buf));
        }
    } catch (const std::exception& e) {
        std::cout << "Erro ao desserializar lista: " << e.what() << std::endl;
    }
    
    return result;
}

// Função para mostrar lista de funcionários
void mostrarListaFuncionarios(const std::vector<char>& data) {
    if (data.empty()) {
        std::cout << "Lista vazia" << std::endl;
        return;
    }
    
    try {
        auto funcionarios = unpackFuncionarioList(data);
        std::cout << "Encontrados " << funcionarios.size() << " funcionários:" << std::endl;
        for (const auto& func : funcionarios) {
            func->exibirInfo();
            std::cout << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "Erro ao decodificar lista: " << e.what() << std::endl;
        std::cout << "Dados recebidos: " << data.size() << " bytes" << std::endl;
    }
}

int main() {
    try {
        std::cout << "=== CLIENTE RMI - TESTES COMPLETOS ===" << std::endl;
        
        RMIClient client("127.0.0.1", 9090);
        RemoteObjectRef remoteObj("SupermercadoService", "127.0.0.1", 9090);
        std::vector<char> emptyArgs;
        
        // TESTE 1: Calcular folha de pagamento
        std::cout << "\n TESTE 1: calcularFolhaPagamento" << std::endl;
        std::vector<char> result = client.doOperation(remoteObj, "calcularFolhaPagamento", emptyArgs);
        double folha = deserializeDouble(result);
        std::cout << " Folha de pagamento total: R$ " << std::fixed << folha << std::endl;
        
        // TESTE 2: Listar funcionários
        std::cout << "\n TESTE 2: listarFuncionarios" << std::endl;
        result = client.doOperation(remoteObj, "listarFuncionarios", emptyArgs);
        mostrarListaFuncionarios(result);
        
        // TESTE 3: Adicionar novo funcionário
        std::cout << "\n TESTE 3: adicionarFuncionario" << std::endl;
        Vendedor novoFunc(5, "Pedro Albuquerque", 3000.0, 250.0);
        std::vector<char> funcData = serializarFuncionarioParaEnvio(novoFunc);
        result = client.doOperation(remoteObj, "adicionarFuncionario", funcData);
        bool sucesso = deserializeBool(result);
        std::cout << "Novo funcionário adicionado: " << (sucesso ? " SIM" : " NÃO") << std::endl;
        
        // TESTE 4: Buscar por tipo
        std::cout << "\n TESTE 4: getFuncionariosPorTipo" << std::endl;
        std::string tipo = "Vendedor";
        std::vector<char> tipoArgs(tipo.begin(), tipo.end());
        result = client.doOperation(remoteObj, "getFuncionariosPorTipo", tipoArgs);
        std::cout << "Vendedores encontrados:" << std::endl;
        mostrarListaFuncionarios(result);
        
        // TESTE 5: Recalcular folha após adição
        std::cout << "\n TESTE 5: calcularFolhaPagamento (após adição)" << std::endl;
        result = client.doOperation(remoteObj, "calcularFolhaPagamento", emptyArgs);
        folha = deserializeDouble(result);
        std::cout << "Folha de pagamento atualizada: R$ " << std::fixed << folha << std::endl;
        
        std::cout << "\n TODOS OS TESTES CONCLUÍDOS!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "ERRO: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}