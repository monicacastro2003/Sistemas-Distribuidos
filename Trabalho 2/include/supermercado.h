#ifndef SUPERMERCADO_H
#define SUPERMERCADO_H

#include <vector>
#include <memory>
#include "funcionario.h"

class Supermercado {
    std::vector<std::shared_ptr<Funcionario>> funcionarios;
    std::string nome;
public:
    Supermercado(const std::string &nome_ = "MeuSuper") : nome(nome_) {}
    void adicionar(const std::shared_ptr<Funcionario> &f) {
        funcionarios.push_back(f);
    }
    void listar(std::ostream &os = std::cout) const {
        os << "Supermercado: " << nome << " - Lista de funcionarios:\n";
        for (const auto &f : funcionarios) {
            f->exibirInfo(os);
            os << " | Tipo: " << f->tipo() << " | Escala: " << f->mostrarEscala() << "\n";
        }
    }
    const std::vector<std::shared_ptr<Funcionario>>& getFuncionarios() const {
        return funcionarios;
    }
};

#endif