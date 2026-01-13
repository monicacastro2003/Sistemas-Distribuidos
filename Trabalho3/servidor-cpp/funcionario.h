#ifndef FUNCIONARIO_H
#define FUNCIONARIO_H

#include <string>
#include <iostream>
#include <memory>

class Funcionario {
protected:
    int id;
    std::string nome;
    double salario;
    
public:
    Funcionario();
    Funcionario(int id, const std::string &nome, double salario);
    virtual ~Funcionario() = default;
    
    // Métodos existentes
    virtual std::string tipo() const = 0;
    virtual void exibirInfo(std::ostream &os = std::cout) const;
    virtual std::string mostrarEscala() const = 0;
    
    // Novos métodos para serialização JSON
    virtual std::string toJson() const;
    virtual void fromJson(const std::string& jsonStr);
    
    // Getters e Setters
    int getId() const { return id; }
    void setId(int id) { this->id = id; }
    
    std::string getNome() const { return nome; }
    void setNome(const std::string& nome) { this->nome = nome; }
    
    double getSalario() const { return salario; }
    void setSalario(double salario) { this->salario = salario; }
    
    // Factory method para criar a partir do tipo
    static std::shared_ptr<Funcionario> createFromType(const std::string& tipo);
};

#endif