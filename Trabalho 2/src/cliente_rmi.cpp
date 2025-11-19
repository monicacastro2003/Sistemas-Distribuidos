#include "../include/rmi_core.h"
#include "../include/subclasses.h"
#include "../include/serializer.h"
#include <iostream>
#include <cstring>  
#include <sstream>

// Função auxiliar simples para desserializar double
double deserializeDouble(const std::vector<char>& data) {
    double value;
    memcpy(&value, data.data(), sizeof(value));
    return value;
}

// Função auxiliar simples para desserializar bool
bool deserializeBool(const std::vector<char>& data) {
    return data[0] != 0;
}

// Função para serializar um funcionário
std::vector<char> serializeFuncionario(const Funcionario& func) {
    // Usando a função do serializer se disponível, senão cria uma simples
    std::ostringstream os(std::ios::binary);
    
    // Serialização simples para teste
    std::string tipo = func.tipo();
    int32_t tipo_size = tipo.size();
    os.write(reinterpret_cast<const char*>(&tipo_size), sizeof(tipo_size));
    os.write(tipo.c_str(), tipo.size());
    
    int32_t id = func.getId();
    os.write(reinterpret_cast<const char*>(&id), sizeof(id));
    
    std::string nome = func.getNome();
    int32_t nome_size = nome.size();
    os.write(reinterpret_cast<const char*>(&nome_size), sizeof(nome_size));
    os.write(nome.c_str(), nome.size());
    
    double salario = func.getSalario();
    os.write(reinterpret_cast<const char*>(&salario), sizeof(salario));
    
    std::string str = os.str();
    return std::vector<char>(str.begin(), str.end());
}

int main() {
    try {
        std::cout << "=== CLIENTE RMI SUPERMERCADO - TESTE SIMPLIFICADO ===" << std::endl;
        
        RMIClient client("127.0.0.1", 9090);
        RemoteObjectRef remoteObj("SupermercadoService", "127.0.0.1", 9090);
        
        // Teste 1: Calcular folha de pagamento (não precisa de argumentos)
        std::cout << "\n1. Testando calcularFolhaPagamento..." << std::endl;
        std::vector<char> emptyArgs;
        std::vector<char> result = client.doOperation(remoteObj, "calcularFolhaPagamento", emptyArgs);
        double folha = deserializeDouble(result);
        std::cout << "Folha de pagamento total: R$ " << folha << std::endl;
        
        // Teste 2: Tentar adicionar um funcionário
        std::cout << "\n2. Testando adicionarFuncionario..." << std::endl;
        Vendedor novoVendedor(10, "Carlos Silva", 2800.0, 200.0);
        std::vector<char> funcData = serializeFuncionario(novoVendedor);
        result = client.doOperation(remoteObj, "adicionarFuncionario", funcData);
        bool sucesso = deserializeBool(result);
        std::cout << "Funcionário adicionado: " << (sucesso ? "SIM" : "NÃO") << std::endl;
        
        // Teste 3: Buscar por tipo
        std::cout << "\n3. Testando getFuncionariosPorTipo..." << std::endl;
        std::string tipo = "Vendedor";
        std::vector<char> tipoArgs(tipo.begin(), tipo.end());
        result = client.doOperation(remoteObj, "getFuncionariosPorTipo", tipoArgs);
        std::cout << "Resposta recebida com " << result.size() << " bytes" << std::endl;
        
        std::cout << "\n=== TODOS OS TESTES CONCLUÍDOS ===" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Erro no cliente: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}