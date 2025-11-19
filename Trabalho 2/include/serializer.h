#ifndef SERIALIZER_H
#define SERIALIZER_H

#include "funcionario.h"
#include <ostream>
#include <istream>
#include <vector>
#include <cstdint>
#include <memory>


// Funções para enviar/receber todos os bytes (usadas com sockets)
ssize_t send_all(int sockfd, const void *buf, size_t len);
ssize_t recv_all(int sockfd, void *buf, size_t len);

// Serializador para Stream (stdout/file) e para sockets (via helpers)
class Serializer {
public:
    // Serializar uma lista de funcionários para um std::ostream (arquivo ou std::cout)
    static void writeToStream(const std::vector<std::shared_ptr<Funcionario>>& list, std::ostream &os);

    // Ler a lista de um std::istream (arquivo ou stdin)
    static std::vector<std::shared_ptr<Funcionario>> readFromStream(std::istream &is);

    // Serializar para socket (envia número de objetos, depois cada objeto)
    static bool writeToSocket(int sockfd, const std::vector<std::shared_ptr<Funcionario>>& list);

    // Ler de socket: retorna vetor reconstruído
    static std::vector<std::shared_ptr<Funcionario>> readFromSocket(int sockfd);
};

// Funções para serialização individual de Funcionario
std::vector<char> packFuncionario(const Funcionario& f);
std::shared_ptr<Funcionario> unpackFuncionario(const std::vector<char>& buf);

#endif