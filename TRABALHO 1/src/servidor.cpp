#include "../include/serializer.h"
#include "../include/subclasses.h"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <thread>
#include <vector>

using namespace std;

void handleClient(int clientSock) {
    cout << "\n==============================================" << endl;
    cout << "         NOVO CLIENTE CONECTADO" << endl;
    cout << "==============================================\n" << endl;

    auto funcionarios = Serializer::readFromSocket(clientSock);
    if (funcionarios.empty()) {
        cerr << "Nenhum dado recebido ou erro na leitura do socket." << endl;
        close(clientSock);
        return;
    }

    cout << "Dados recebidos do cliente com sucesso!" << endl;
    cout << "\n Supermercado: Central" << endl;
    cout << "==============================================" << endl;
    cout << "Lista de Funcionários Recebidos:" << endl;
    cout << "----------------------------------------------" << endl;

    for (const auto &f : funcionarios) {
        f->exibirInfo();
        cout << endl;
        cout << "----------------------------------------------" << endl;
    }

    cout << "Todos os funcionários foram exibidos corretamente.\n" << endl;

    // Envia confirmação ao cliente
    int32_t resp = htonl(static_cast<int32_t>(funcionarios.size()));
    send(clientSock, &resp, sizeof(resp), 0);

    cout << "Confirmação enviada ao cliente: "
         << funcionarios.size() << " funcionários recebidos." << endl;

    cout << "\n==============================================" << endl;
    cout << "     Conexão com cliente encerrada" << endl;
    cout << "==============================================\n" << endl;

    close(clientSock);
}

int main() {
    int serverSock = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSock < 0) {
        perror("Erro ao criar socket");
        return 1;
    }

    sockaddr_in serv{};
    serv.sin_family = AF_INET;
    serv.sin_port = htons(9090);
    serv.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSock, (sockaddr*)&serv, sizeof(serv)) < 0) {
        perror("Erro no bind");
        close(serverSock);
        return 1;
    }

    if (listen(serverSock, 5) < 0) {
        perror("Erro no listen");
        close(serverSock);
        return 1;
    }

    cout << "==============================================" << endl;
    cout << "       SERVIDOR AGUARDANDO CONEXÕES..." << endl;
    cout << "==============================================\n" << endl;

    while (true) {
        sockaddr_in clientAddr{};
        socklen_t len = sizeof(clientAddr);
        int clientSock = accept(serverSock, (sockaddr*)&clientAddr, &len);
        if (clientSock < 0) {
            perror("Erro no accept");
            continue;
        }

        thread(handleClient, clientSock).detach();
    }

    close(serverSock);
    return 0;
}
