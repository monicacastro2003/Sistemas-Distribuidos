#include "../include/rmi_core.h"
#include "../include/remote_service_impl.h"
#include "../include/subclasses.h"
#include "../include/serializer.h"
#include <iostream>
#include <memory>

// Função helper para criar e adicionar funcionários de teste
void adicionarDadosTeste(RemoteSupermercadoService* service) {
    std::cout << "Criando dados de teste..." << std::endl;
    
    // Cria funcionários de exemplo
    Vendedor vendedor(1, "Maria Silva", 2500.0, 150.0);
    Gerente gerente(2, "João Santos", 5000.0, 700.0);
    Caixa caixa(3, "Carlos Oliveira", 1800.0, 2);
    Balconista balconista(4, "Ana Costa", 2200.0, "Padaria");
    
    // Serializa e adiciona via serviço
    std::vector<char> data1 = packFuncionario(vendedor);
    std::vector<char> data2 = packFuncionario(gerente);
    std::vector<char> data3 = packFuncionario(caixa);
    std::vector<char> data4 = packFuncionario(balconista);
    
    bool success1 = service->adicionarFuncionario(data1);
    bool success2 = service->adicionarFuncionario(data2);
    bool success3 = service->adicionarFuncionario(data3);
    bool success4 = service->adicionarFuncionario(data4);
    
    std::cout << "Dados de teste adicionados: " 
              << (success1 && success2 && success3 && success4 ? "SUCESSO" : "FALHA") 
              << std::endl;
    std::cout << "4 funcionários criados:" << std::endl;
    std::cout << " - Vendedor: Maria Silva, R$ 2500 + R$ 150 comissão" << std::endl;
    std::cout << " - Gerente: João Santos, R$ 5000 + R$ 700 bônus" << std::endl;
    std::cout << " - Caixa: Carlos Oliveira, R$ 1800, Caixa 2" << std::endl;
    std::cout << " - Balconista: Ana Costa, R$ 2200, Setor Padaria" << std::endl;
}

int main() {
    std::cout << "=== INICIANDO SERVIDOR RMI SUPERMERCADO ===" << std::endl;
    
    // Cria e configura o serviço remoto
    RemoteSupermercadoService* service = new RemoteSupermercadoService("Supermercado Central");
    std::cout << "Serviço remoto criado." << std::endl;
    
    // Adiciona dados de teste
    adicionarDadosTeste(service);
    
    // Configura e inicia o servidor RMI
    RMIServer server(9090);
    std::cout << "Servidor RMI configurado na porta 9090." << std::endl;
    
    if (!server.start()) {
        std::cerr << "FALHA ao iniciar servidor!" << std::endl;
        delete service;
        return 1;
    }
    
    std::cout << "Servidor socket iniciado com sucesso." << std::endl;
    
    // Registra o objeto remoto
    server.registerObject("SupermercadoService", service);
    std::cout << "Objeto remoto 'SupermercadoService' registrado." << std::endl;
    
    std::cout << "\n--- MÉTODOS DISPONÍVEIS ---" << std::endl;
    std::cout << "• adicionarFuncionario" << std::endl;
    std::cout << "• listarFuncionarios" << std::endl;
    std::cout << "• buscarFuncionarioPorId" << std::endl;
    std::cout << "• calcularFolhaPagamento" << std::endl;
    std::cout << "• getFuncionariosPorTipo" << std::endl;
    
    std::cout << "\n=== SERVIDOR AGUARDANDO CONEXÕES CLIENTES ===" << std::endl;
    
    // Inicia o loop principal
    server.run();
    
    delete service;
    return 0;
}