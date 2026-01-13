#ifndef SUBCLASSES_H
#define SUBCLASSES_H

#include "funcionario.h"

// Vendedor
class Vendedor : public Funcionario {
    double comissao;
public:
    Vendedor() : Funcionario(), comissao(0.0) {}
    Vendedor(int id, const std::string &n, double s, double c)
        : Funcionario(id, n, s), comissao(c) {}
    std::string tipo() const override { return "Vendedor"; }
    double getComissao() const { return comissao; }
    std::string mostrarEscala() const override {
        return "Vendedor: escala 9h-18h";
    }
    void exibirInfo(std::ostream &os = std::cout) const override {
        os << "Vendedor - ";
        Funcionario::exibirInfo(os);
        os << " | Comissao: " << comissao;
    }
};

// Balconista
class Balconista : public Funcionario {
    std::string setor;
public:
    Balconista() : Funcionario(), setor("") {}
    Balconista(int id, const std::string &n, double s, const std::string &setor_)
        : Funcionario(id, n, s), setor(setor_) {}
    std::string tipo() const override { return "Balconista"; }
    std::string getSetor() const { return setor; }
    std::string mostrarEscala() const override {
        return "Balconista: escala 8h-17h";
    }
    void exibirInfo(std::ostream &os = std::cout) const override {
        os << "Balconista - ";
        Funcionario::exibirInfo(os);
        os << " | Setor: " << setor;
    }
};

// Caixa
class Caixa : public Funcionario {
    int numeroCaixa;
public:
    Caixa() : Funcionario(), numeroCaixa(0) {}
    Caixa(int id, const std::string &n, double s, int num)
        : Funcionario(id, n, s), numeroCaixa(num) {}
    std::string tipo() const override { return "Caixa"; }
    int getNumeroCaixa() const { return numeroCaixa; }
    std::string mostrarEscala() const override {
        return "Caixa: escala 10h-19h";
    }
    void exibirInfo(std::ostream &os = std::cout) const override {
        os << "Caixa - ";
        Funcionario::exibirInfo(os);
        os << " | NumCaixa: " << numeroCaixa;
    }
};

// Gerente
class Gerente : public Funcionario {
    double bonus;
public:
    Gerente() : Funcionario(), bonus(0.0) {}
    Gerente(int id, const std::string &n, double s, double b)
        : Funcionario(id, n, s), bonus(b) {}
    std::string tipo() const override { return "Gerente"; }
    double getBonus() const { return bonus; }
    std::string mostrarEscala() const override {
        return "Gerente: escala administrativa";
    }
    void exibirInfo(std::ostream &os = std::cout) const override {
        os << "Gerente - ";
        Funcionario::exibirInfo(os);
        os << " | Bonus: " << bonus;
    }
};

#endif