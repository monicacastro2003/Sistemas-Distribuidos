#include "../include/serializer.h"
#include "../include/subclasses.h"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <fstream>

using namespace std;

int main() {
    // Cria lista de funcionarios (exemplo)
    vector<shared_ptr<Funcionario>> list;
    list.push_back(make_shared<Vendedor>(1, "Maria", 2500.5, 150.0));
    list.push_back(make_shared<Caixa>(2, "Joao", 1800.75, 3));
    list.push_back(make_shared<Balconista>(3, "Carla", 2000.0, "Padaria"));
    list.push_back(make_shared<Gerente>(4, "Ana", 5000.0, 700.0));

    cout << "\n==============================================" << endl;
    cout << "        TESTE 1 - ESCRITA LEGÍVEL (STDOUT)" << endl;
    cout << "==============================================\n" << endl;

    for (const auto &f : list) {
        f->exibirInfo();
        cout << endl;
    }

    cout << "\n==============================================" << endl;
    cout << "        TESTE 2 - ESCRITA BINÁRIA (ARQUIVO)" << endl;
    cout << "==============================================\n" << endl;

    {
        ofstream ofs("funcs.bin", ios::binary);
        Serializer::writeToStream(list, ofs);
    }

    cout << "Arquivo 'funcs.bin' gravado com sucesso (modo binário)." << endl;

    cout << "\n==============================================" << endl;
    cout << "           TESTE 3 - ENVIO VIA TCP" << endl;
    cout << "==============================================\n" << endl;

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Erro ao criar socket");
        return 1;
    }

    sockaddr_in serv{};
    serv.sin_family = AF_INET;
    serv.sin_port = htons(9090);
    serv.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sock, (sockaddr*)&serv, sizeof(serv)) < 0) {
        perror("Erro ao conectar ao servidor");
        close(sock);
        return 1;
    }

    bool ok = Serializer::writeToSocket(sock, list);
    if (!ok) {
        cerr << "Erro ao enviar dados para o servidor!" << endl;
        close(sock);
        return 1;
    }

    cout << "Dados enviados ao servidor via TCP com sucesso.\n" << endl;

    // Ler resposta: número de objetos recebidos
    int32_t resp_net;
    if (recv(sock, &resp_net, sizeof(resp_net), 0) > 0) {
        int32_t resp = ntohl(resp_net);
        cout << "Servidor confirmou recebimento de " << resp << " funcionários." << endl;
    }

    cout << "\n==============================================" << endl;
    cout << "        CLIENTE FINALIZADO COM SUCESSO" << endl;
    cout << "==============================================\n" << endl;

    close(sock);
    return 0;
}
