#include "funcionario.h"

Funcionario::Funcionario() : id(0), salario(0.0) {}
Funcionario::Funcionario(int id, const std::string &nome, double salario) 
    : id(id), nome(nome), salario(salario) {}

void Funcionario::exibirInfo(std::ostream &os) const {
    os << "ID: " << id << " | Nome: " << nome << " | Salario: " << salario;
}

std::string Funcionario::toJson() const {
    return "{\"tipo\":\"FuncionarioBase\"}";
}

void Funcionario::fromJson(const std::string& jsonStr) {
    // Implementação básica
}

std::shared_ptr<Funcionario> Funcionario::createFromType(const std::string& tipo) {
    return nullptr; // Implementação básica
}