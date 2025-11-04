#include "serializer.h"
#include "subclasses.h"
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <sstream>
#include <memory>
#include <iostream>

using namespace std;

// Funções auxiliares de leitura/escrita de inteiros em streams
static void write_int32_to_stream(ostream &os, int32_t v) {
    os.write(reinterpret_cast<const char*>(&v), sizeof(v));
}
static bool read_int32_from_stream(istream &is, int32_t &v) {
    is.read(reinterpret_cast<char*>(&v), sizeof(v));
    return (bool)is;
}

// Funções auxiliares para enviar/receber todos os bytes via socket
ssize_t send_all(int sockfd, const void *buf, size_t len) {
    size_t total = 0;
    const char *p = (const char*)buf;
    while (total < len) {
        ssize_t sent = send(sockfd, p + total, len - total, 0);
        if (sent <= 0) return sent;
        total += sent;
    }
    return total;
}
ssize_t recv_all(int sockfd, void *buf, size_t len) {
    size_t total = 0;
    char *p = (char*)buf;
    while (total < len) {
        ssize_t r = recv(sockfd, p + total, len - total, 0);
        if (r <= 0) return r;
        total += r;
    }
    return total;
}

// Empacotamento (serialização) de um objeto Funcionario
static std::vector<char> packFuncionario(const Funcionario &f) {
    std::ostringstream os(std::ios::binary);
    // tipo (string)
    std::string tipo = f.tipo();
    uint8_t tipo_len = (uint8_t)std::min<size_t>(tipo.size(), 255);
    os.write(reinterpret_cast<const char*>(&tipo_len), sizeof(tipo_len));
    os.write(tipo.c_str(), tipo_len);

    // id
    int32_t id = f.getId();
    os.write(reinterpret_cast<const char*>(&id), sizeof(id));

    // nome
    std::string nome = f.getNome();
    int32_t nome_len = (int32_t)nome.size();
    os.write(reinterpret_cast<const char*>(&nome_len), sizeof(nome_len));
    os.write(nome.c_str(), nome_len);

    // salario
    double s = f.getSalario();
    os.write(reinterpret_cast<const char*>(&s), sizeof(s));

    // campos extras por tipo (simplesmente colocamos como string chave=valor; opcional)
    std::string extras;
    if (tipo == "Vendedor") {
        auto *p = dynamic_cast<const Vendedor*>(&f);
        if (p) {
            extras = "comissao=" + std::to_string(p->getComissao());
        }
    } else if (tipo == "Gerente") {
        auto *p = dynamic_cast<const Gerente*>(&f);
        if (p) extras = "bonus=" + std::to_string(p->getBonus());
    } else if (tipo == "Caixa") {
        auto *p = dynamic_cast<const Caixa*>(&f);
        if (p) extras = "numcaixa=" + std::to_string(p->getNumeroCaixa());
    } else if (tipo == "Balconista") {
        auto *p = dynamic_cast<const Balconista*>(&f);
        if (p) extras = "setor=" + p->getSetor();
    }
    int32_t extras_len = (int32_t)extras.size();
    os.write(reinterpret_cast<const char*>(&extras_len), sizeof(extras_len));
    if (extras_len > 0) os.write(extras.c_str(), extras_len);

    std::string str = os.str();
    return std::vector<char>(str.begin(), str.end());
}

// Desempacotamento (desserialização)
static std::shared_ptr<Funcionario> unpackFuncionario(const std::vector<char> &buf) {
    std::istringstream is(std::string(buf.data(), buf.size()), std::ios::binary);
    uint8_t tipo_len;
    is.read(reinterpret_cast<char*>(&tipo_len), sizeof(tipo_len));
    std::string tipo(tipo_len, '\0');
    is.read(&tipo[0], tipo_len);

    int32_t id;
    is.read(reinterpret_cast<char*>(&id), sizeof(id));

    int32_t nome_len;
    is.read(reinterpret_cast<char*>(&nome_len), sizeof(nome_len));
    std::string nome(nome_len, '\0');
    if (nome_len > 0) is.read(&nome[0], nome_len);

    double salario;
    is.read(reinterpret_cast<char*>(&salario), sizeof(salario));

    int32_t extras_len;
    is.read(reinterpret_cast<char*>(&extras_len), sizeof(extras_len));
    std::string extras(extras_len, '\0');
    if (extras_len > 0) is.read(&extras[0], extras_len);

    // Criação do objeto correto com base no tipo
    if (tipo == "Vendedor") {
        double comissao = 0.0;
        if (!extras.empty()) {
            auto pos = extras.find("comissao=");
            if (pos != std::string::npos) comissao = std::stod(extras.substr(pos+9));
        }
        return std::make_shared<Vendedor>(id, nome, salario, comissao);
    } else if (tipo == "Gerente") {
        double bonus = 0.0;
        if (!extras.empty()) {
            auto pos = extras.find("bonus=");
            if (pos != std::string::npos) bonus = std::stod(extras.substr(pos+6));
        }
        return std::make_shared<Gerente>(id, nome, salario, bonus);
    } else if (tipo == "Caixa") {
        int n = 0;
        if (!extras.empty()) {
            auto pos = extras.find("numcaixa=");
            if (pos != std::string::npos) n = std::stoi(extras.substr(pos+9));
        }
        return std::make_shared<Caixa>(id, nome, salario, n);
    } else if (tipo == "Balconista") {
        std::string setor;
        if (!extras.empty()) {
            auto pos = extras.find("setor=");
            if (pos != std::string::npos) setor = extras.substr(pos+6);
        }
        return std::make_shared<Balconista>(id, nome, salario, setor);
    } else {
        return std::make_shared<Funcionario>(id, nome, salario);
    }
}

//Escrita e leitura em streams (ex: arquivo binário)
// Grava a lista de funcionários em um arquivo ou fluxo binário
void Serializer::writeToStream(const vector<shared_ptr<Funcionario>>& list, ostream &os) {
    int32_t n = (int32_t)list.size();
    os.write(reinterpret_cast<const char*>(&n), sizeof(n));
    for (const auto &fptr : list) {
        auto buf = packFuncionario(*fptr);
        int32_t bytes = (int32_t)buf.size();
        os.write(reinterpret_cast<const char*>(&bytes), sizeof(bytes)); // bytes usados
        os.write(buf.data(), buf.size());
    }
}

// Lê a lista de funcionários de um arquivo binário
vector<shared_ptr<Funcionario>> Serializer::readFromStream(istream &is) {
    vector<shared_ptr<Funcionario>> res;
    int32_t n;
    is.read(reinterpret_cast<char*>(&n), sizeof(n));
    for (int i = 0; i < n; ++i) {
        int32_t bytes;
        is.read(reinterpret_cast<char*>(&bytes), sizeof(bytes));
        vector<char> buf(bytes);
        is.read(buf.data(), bytes);
        res.push_back(unpackFuncionario(buf));
    }
    return res;
}

// Escrita e leitura via SOCKET TCP
bool Serializer::writeToSocket(int sockfd, const vector<shared_ptr<Funcionario>>& list) {
    int32_t n = (int32_t)list.size();
    int32_t n_net = htonl(n);
    if (send_all(sockfd, &n_net, sizeof(n_net)) <= 0) return false;
    for (const auto &fptr : list) {
        auto buf = packFuncionario(*fptr);
        int32_t bytes = (int32_t)buf.size();
        int32_t bytes_net = htonl(bytes);
        if (send_all(sockfd, &bytes_net, sizeof(bytes_net)) <= 0) return false;
        if (send_all(sockfd, buf.data(), buf.size()) <= 0) return false;
    }
    return true;
}

vector<shared_ptr<Funcionario>> Serializer::readFromSocket(int sockfd) {
    vector<shared_ptr<Funcionario>> res;
    int32_t n_net;
    if (recv_all(sockfd, &n_net, sizeof(n_net)) <= 0) return res;
    int32_t n = ntohl(n_net);
    for (int i = 0; i < n; ++i) {
        int32_t bytes_net;
        if (recv_all(sockfd, &bytes_net, sizeof(bytes_net)) <= 0) return res;
        int32_t bytes = ntohl(bytes_net);
        vector<char> buf(bytes);
        if (recv_all(sockfd, buf.data(), bytes) <= 0) return res;
        res.push_back(unpackFuncionario(buf));
    }
    return res;
}
