#ifndef FUNCIONARIO_H
#define FUNCIONARIO_H

#include <string>
#include <iostream>

class Escala { // "interface" Escala
public:
    virtual ~Escala() = default;
    virtual std::string mostrarEscala() const = 0;
};

class Funcionario : public Escala {
protected:
    int id;
    std::string nome;
    double salario;
public:
    Funcionario() : id(0), nome(""), salario(0.0) {}
    Funcionario(int id_, const std::string &nome_, double salario_)
        : id(id_), nome(nome_), salario(salario_) {}
    virtual ~Funcionario() = default;

    int getId() const { return id; }
    std::string getNome() const { return nome; }
    double getSalario() const { return salario; }

    virtual std::string tipo() const { return "Funcionario"; }

    virtual void exibirInfo(std::ostream &os = std::cout) const {
        os << "ID: " << id << " | Nome: " << nome << " | Salario: " << salario;
    }

    std::string mostrarEscala() const override {
        return "Funcionario: escala padrão 8h-17h";
    }
};


#endif
