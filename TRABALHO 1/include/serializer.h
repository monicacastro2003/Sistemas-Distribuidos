#ifndef SERIALIZER_H
#define SERIALIZER_H

#include "funcionario.h"
#include <ostream>
#include <istream>
#include <vector>
#include <cstdint>
#include <memory>

// Um formato simples de serialização "manual" (padrão):
// [bytes_obj(int32)] [tipo_len(uint8)] [tipo chars] [id(int32)] [nome_len(int32)] [nome chars] [salario(double)]
// Em subclasses, podem vir campos extras (por exemplo comissão) — para simplificar, incluiremos um campo extra string/json opcional.

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

#endif
