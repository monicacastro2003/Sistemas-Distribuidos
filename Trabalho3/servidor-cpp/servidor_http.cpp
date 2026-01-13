#include "supermercado.h"
#include "subclasses.h"
#include "httplib.h"
#include <iostream>
#include <sstream>
#include <memory>
#include <vector>
#include <regex>
#include <set>

using namespace httplib;
using namespace std;

// Variável global do supermercado
Supermercado supermercado("Supermercado UFC");

// Contador de IDs
int proximoId = 5; // Começa em 5 porque já temos 4 funcionários

// Função para limpar e normalizar JSON
string normalizarJson(const string& jsonStr) {
    string resultado;
    bool dentroString = false;
    bool escape = false;
    
    for (size_t i = 0; i < jsonStr.length(); i++) {
        char c = jsonStr[i];
        
        if (escape) {
            resultado += c;
            escape = false;
            continue;
        }
        
        if (c == '\\') {
            escape = true;
            resultado += c;
            continue;
        }
        
        if (c == '\"') {
            dentroString = !dentroString;
        }
        
        // Remove espaços em branco fora de strings
        if (!dentroString && (c == ' ' || c == '\n' || c == '\r' || c == '\t')) {
            continue;
        }
        
        resultado += c;
    }
    
    return resultado;
}

void inicializarDados() {
    cout << "=== INICIALIZANDO SERVIDOR SUPERMERCADO API ===" << endl;
    cout << "Adicionando funcionarios de teste..." << endl;
    
    supermercado.adicionar(make_shared<Vendedor>(1, "Maria Silva", 2500.0, 150.0));
    supermercado.adicionar(make_shared<Gerente>(2, "João Santos", 5000.0, 700.0));
    supermercado.adicionar(make_shared<Caixa>(3, "Carlos Oliveira", 1800.0, 2));
    supermercado.adicionar(make_shared<Balconista>(4, "Ana Costa", 2200.0, "Padaria"));
    
    cout << "✓ 4 funcionarios adicionados" << endl;
}

string funcionarioParaJson(shared_ptr<Funcionario> f) {
    if (!f) return "{}";
    
    stringstream json;
    json << "{";
    json << "\"id\":" << f->getId() << ",";
    json << "\"nome\":\"" << f->getNome() << "\",";
    json << "\"salario\":" << f->getSalario() << ",";
    json << "\"tipo\":\"" << f->tipo() << "\"";
    
    if (f->tipo() == "Vendedor") {
        auto v = dynamic_pointer_cast<Vendedor>(f);
        if (v) json << ",\"comissao\":" << v->getComissao();
    } else if (f->tipo() == "Gerente") {
        auto g = dynamic_pointer_cast<Gerente>(f);
        if (g) json << ",\"bonus\":" << g->getBonus();
    } else if (f->tipo() == "Caixa") {
        auto c = dynamic_pointer_cast<Caixa>(f);
        if (c) json << ",\"numeroCaixa\":" << c->getNumeroCaixa();
    } else if (f->tipo() == "Balconista") {
        auto b = dynamic_pointer_cast<Balconista>(f);
        if (b) json << ",\"setor\":\"" << b->getSetor() << "\"";
    }
    
    json << "}";
    return json.str();
}

string funcionariosParaJson() {
    auto funcionarios = supermercado.getFuncionarios();
    stringstream json;
    json << "[";
    
    for (size_t i = 0; i < funcionarios.size(); i++) {
        json << funcionarioParaJson(funcionarios[i]);
        if (i < funcionarios.size() - 1) json << ",";
    }
    
    json << "]";
    return json.str();
}

// Função auxiliar para extrair valores do JSON
string extrairValorJson(const string& jsonStr, const string& campo) {
    // Primeiro tenta com o campo seguido de aspas (string)
    string padrao = "\"" + campo + "\":\"([^\"]*)\"";
    regex regexPadrao(padrao);
    smatch matches;
    
    if (regex_search(jsonStr, matches, regexPadrao) && matches.size() > 1) {
        return matches[1];
    }
    
    // Tenta com o campo seguido de número decimal
    padrao = "\"" + campo + "\":([0-9]*\\.?[0-9]+)";
    regex regexPadrao2(padrao);
    if (regex_search(jsonStr, matches, regexPadrao2) && matches.size() > 1) {
        return matches[1];
    }
    
    // Tenta com o campo seguido de número inteiro
    padrao = "\"" + campo + "\":([0-9]+)";
    regex regexPadrao3(padrao);
    if (regex_search(jsonStr, matches, regexPadrao3) && matches.size() > 1) {
        return matches[1];
    }
    
    return "";
}

// Função melhorada para criar funcionário a partir de JSON
shared_ptr<Funcionario> criarFuncionarioDeJson(const string& jsonStr) {
    try {
        // Normalizar o JSON primeiro
        string jsonNormalizado = normalizarJson(jsonStr);
        
        if (jsonNormalizado.empty()) {
            cout << "  ❌ JSON vazio recebido" << endl;
            return nullptr;
        }
        
        cout << "  🔍 JSON normalizado: " << jsonNormalizado << endl;
        
        // Extrai valores
        string nome = extrairValorJson(jsonNormalizado, "nome");
        string salarioStr = extrairValorJson(jsonNormalizado, "salario");
        string tipo = extrairValorJson(jsonNormalizado, "tipo");
        
        if (nome.empty()) {
            cout << "  ❌ Campo 'nome' não encontrado ou vazio" << endl;
            return nullptr;
        }
        if (salarioStr.empty()) {
            cout << "  ❌ Campo 'salario' não encontrado ou inválido" << endl;
            return nullptr;
        }
        if (tipo.empty()) {
            cout << "  ❌ Campo 'tipo' não encontrado ou vazio" << endl;
            return nullptr;
        }
        
        double salario = stod(salarioStr);
        int novoId = proximoId++;
        shared_ptr<Funcionario> novoFunc;
        
        cout << "  ✓ Campos extraídos: Nome=" << nome << ", Salario=" << salario << ", Tipo=" << tipo << endl;
        
        if (tipo == "Vendedor") {
            string comissaoStr = extrairValorJson(jsonNormalizado, "comissao");
            double comissao = comissaoStr.empty() ? 0.0 : stod(comissaoStr);
            novoFunc = make_shared<Vendedor>(novoId, nome, salario, comissao);
            cout << "  ✓ Criado Vendedor com comissão: " << comissao << endl;
            
        } else if (tipo == "Gerente") {
            string bonusStr = extrairValorJson(jsonNormalizado, "bonus");
            double bonus = bonusStr.empty() ? 0.0 : stod(bonusStr);
            novoFunc = make_shared<Gerente>(novoId, nome, salario, bonus);
            cout << "  ✓ Criado Gerente com bônus: " << bonus << endl;
            
        } else if (tipo == "Caixa") {
            string numCaixaStr = extrairValorJson(jsonNormalizado, "numeroCaixa");
            int numCaixa = numCaixaStr.empty() ? 1 : stoi(numCaixaStr);
            novoFunc = make_shared<Caixa>(novoId, nome, salario, numCaixa);
            cout << "  ✓ Criado Caixa com número: " << numCaixa << endl;
            
        } else if (tipo == "Balconista") {
            string setor = extrairValorJson(jsonNormalizado, "setor");
            if (setor.empty()) setor = "Geral";
            novoFunc = make_shared<Balconista>(novoId, nome, salario, setor);
            cout << "  ✓ Criado Balconista no setor: " << setor << endl;
        } else {
            cout << "  ❌ Tipo de funcionário inválido: " << tipo << endl;
            cout << "  Tipos válidos: Vendedor, Gerente, Caixa, Balconista" << endl;
            return nullptr;
        }
        
        return novoFunc;
        
    } catch (const exception& e) {
        cout << "  ❌ ERRO FATAL ao processar JSON: " << e.what() << endl;
        cout << "  JSON recebido original: " << jsonStr << endl;
        return nullptr;
    }
}

int main() {
    cout << "==================================================" << endl;
    cout << "        SERVIDOR HTTP - SUPERMERCADO API" << endl;
    cout << "        Trabalho 3 - Sistemas Distribuídos" << endl;
    cout << "==================================================" << endl;
    
    inicializarDados();
    
    Server svr;
    
    // ==================== API ENDPOINTS ====================
    
    // GET /api/funcionarios - Listar todos
    svr.Get("/api/funcionarios", [](const Request& req, Response& res) {
        cout << "📋 GET /api/funcionarios - Total: " << supermercado.getFuncionarios().size() << " funcionarios" << endl;
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Content-Type", "application/json; charset=utf-8");
        res.set_content(funcionariosParaJson(), "application/json");
    });
    
    // GET /api/funcionarios/folha - Calcular folha
    svr.Get("/api/funcionarios/folha", [](const Request& req, Response& res) {
        cout << "💰 GET /api/funcionarios/folha" << endl;
        
        auto funcionarios = supermercado.getFuncionarios();
        double total = 0;
        for (auto& f : funcionarios) total += f->getSalario();
        
        stringstream json;
        json << "{";
        json << "\"folha_pagamento\":" << total << ",";
        json << "\"quantidade_funcionarios\":" << funcionarios.size() << ",";
        json << "\"media_salarios\":" << (funcionarios.empty() ? 0 : total / funcionarios.size());
        json << "}";
        
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Content-Type", "application/json; charset=utf-8");
        res.set_content(json.str(), "application/json");
    });
    
    // GET /api/funcionarios/tipos - Listar todos os tipos disponíveis
    svr.Get("/api/funcionarios/tipos", [](const Request& req, Response& res) {
        cout << "📊 GET /api/funcionarios/tipos" << endl;
        
        stringstream json;
        json << "{";
        json << "\"tipos_disponiveis\": [\"Vendedor\", \"Gerente\", \"Caixa\", \"Balconista\"],";
        json << "\"descricao_tipos\": {";
        json << "\"Vendedor\": \"Funcionário responsável por vendas, recebe comissão\",";
        json << "\"Gerente\": \"Funcionário responsável pela gerência, recebe bônus\",";
        json << "\"Caixa\": \"Funcionário responsável pelo caixa, possui número de caixa\",";
        json << "\"Balconista\": \"Funcionário responsável pelos setores do supermercado\"";
        json << "}";
        json << "}";
        
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Content-Type", "application/json; charset=utf-8");
        res.set_content(json.str(), "application/json");
    });
    
    // GET /api/funcionarios/tipo/{tipo} - Filtrar por tipo
    svr.Get(R"(/api/funcionarios/tipo/(\w+))", [](const Request& req, Response& res) {
        string tipo = req.matches[1];
        cout << "🔍 GET /api/funcionarios/tipo/" << tipo << endl;
        
        auto todos = supermercado.getFuncionarios();
        stringstream json;
        json << "{";
        json << "\"tipo\": \"" << tipo << "\",";
        json << "\"quantidade\": 0,";
        json << "\"funcionarios\": [";
        
        int contador = 0;
        for (size_t i = 0; i < todos.size(); i++) {
            if (todos[i]->tipo() == tipo) {
                if (contador > 0) json << ",";
                json << funcionarioParaJson(todos[i]);
                contador++;
            }
        }
        json << "],";
        json << "\"quantidade\": " << contador;
        json << "}";
        
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Content-Type", "application/json; charset=utf-8");
        res.set_content(json.str(), "application/json");
    });
    
    // POST /api/funcionarios - Adicionar novo (MELHORADO)
    svr.Post("/api/funcionarios", [](const Request& req, Response& res) {
        cout << "➕ POST /api/funcionarios" << endl;
        cout << "  📦 Dados recebidos: " << req.body << endl;
        
        try {
            auto novoFunc = criarFuncionarioDeJson(req.body);
            if (novoFunc) {
                supermercado.adicionar(novoFunc);
                cout << "  ✅ Funcionario adicionado: " << novoFunc->getNome() 
                     << " (ID: " << novoFunc->getId() << ", Tipo: " << novoFunc->tipo() << ")" << endl;
                
                // Resposta mais detalhada
                string resposta = "{";
                resposta += "\"success\": true,";
                resposta += "\"message\": \"Funcionário cadastrado com sucesso!\",";
                resposta += "\"id\": " + to_string(novoFunc->getId()) + ",";
                resposta += "\"nome\": \"" + novoFunc->getNome() + "\",";
                resposta += "\"tipo\": \"" + novoFunc->tipo() + "\",";
                resposta += "\"salario\": " + to_string(novoFunc->getSalario()) + ",";
                resposta += "\"total_funcionarios\": " + to_string(supermercado.getFuncionarios().size());
                
                // Adiciona campos específicos
                if (novoFunc->tipo() == "Vendedor") {
                    auto v = dynamic_pointer_cast<Vendedor>(novoFunc);
                    if (v) resposta += ",\"comissao\": " + to_string(v->getComissao());
                } else if (novoFunc->tipo() == "Gerente") {
                    auto g = dynamic_pointer_cast<Gerente>(novoFunc);
                    if (g) resposta += ",\"bonus\": " + to_string(g->getBonus());
                } else if (novoFunc->tipo() == "Caixa") {
                    auto c = dynamic_pointer_cast<Caixa>(novoFunc);
                    if (c) resposta += ",\"numeroCaixa\": " + to_string(c->getNumeroCaixa());
                } else if (novoFunc->tipo() == "Balconista") {
                    auto b = dynamic_pointer_cast<Balconista>(novoFunc);
                    if (b) resposta += ",\"setor\": \"" + b->getSetor() + "\"";
                }
                
                resposta += "}";
                
                res.set_header("Access-Control-Allow-Origin", "*");
                res.set_header("Content-Type", "application/json; charset=utf-8");
                res.set_content(resposta, "application/json");
            } else {
                cout << "  ❌ Falha ao criar funcionário" << endl;
                
                string resposta = "{";
                resposta += "\"success\": false,";
                resposta += "\"message\": \"Falha ao cadastrar funcionário. Verifique os dados.\",";
                resposta += "\"dicas\": [";
                resposta += "\"Verifique se todos os campos estão preenchidos\",";
                resposta += "\"Nome, salário e tipo são obrigatórios\",";
                resposta += "\"Tipos válidos: Vendedor, Gerente, Caixa, Balconista\",";
                resposta += "\"Use números para salário, comissão e bônus\"";
                resposta += "]";
                resposta += "}";
                
                res.set_header("Access-Control-Allow-Origin", "*");
                res.set_header("Content-Type", "application/json; charset=utf-8");
                res.set_content(resposta, "application/json");
            }
        } catch (const exception& e) {
            cout << "  ❌ Exceção: " << e.what() << endl;
            
            string resposta = "{";
            resposta += "\"success\": false,";
            resposta += "\"message\": \"Erro interno no servidor: " + string(e.what()) + "\",";
            resposta += "\"dica\": \"Contate o administrador do sistema\"";
            resposta += "}";
            
            res.set_header("Access-Control-Allow-Origin", "*");
            res.status = 500;
            res.set_header("Content-Type", "application/json; charset=utf-8");
            res.set_content(resposta, "application/json");
        }
    });
    
    // ==================== PÁGINAS WEB ====================
    
    // GET /cadastro - Formulário HTML
    svr.Get("/cadastro", [](const Request& req, Response& res) {
        cout << "🌐 GET /cadastro" << endl;
        
        string html = R"(<!DOCTYPE html>
<html lang="pt-BR">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Cadastrar Funcionário - Supermercado API</title>
    <style>
        /* Estilos mantidos do anterior - estão bonitos! */
        body { 
            font-family: 'Arial', sans-serif; 
            margin: 0; 
            padding: 20px; 
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh;
        }
        .container { 
            max-width: 500px; 
            margin: 40px auto; 
            background: white; 
            padding: 40px; 
            border-radius: 15px; 
            box-shadow: 0 10px 30px rgba(0,0,0,0.2); 
        }
        h1 { 
            color: #333; 
            border-bottom: 3px solid #667eea; 
            padding-bottom: 15px; 
            margin-top: 0;
            text-align: center;
        }
        label { 
            display: block; 
            margin: 20px 0 8px; 
            font-weight: bold; 
            color: #555;
        }
        input, select { 
            width: 100%; 
            padding: 12px; 
            border: 2px solid #ddd; 
            border-radius: 8px; 
            box-sizing: border-box; 
            font-size: 16px;
            transition: border-color 0.3s;
        }
        input:focus, select:focus { 
            border-color: #667eea; 
            outline: none;
            box-shadow: 0 0 0 3px rgba(102, 126, 234, 0.1);
        }
        button { 
            background: linear-gradient(to right, #667eea, #764ba2); 
            color: white; 
            padding: 15px 30px; 
            border: none; 
            border-radius: 8px; 
            cursor: pointer; 
            font-size: 18px; 
            font-weight: bold;
            margin-top: 30px; 
            width: 100%;
            transition: transform 0.2s, box-shadow 0.2s;
        }
        button:hover { 
            transform: translateY(-2px);
            box-shadow: 0 5px 15px rgba(102, 126, 234, 0.4);
        }
        button:active {
            transform: translateY(0);
        }
        #resultado { 
            margin-top: 25px; 
            padding: 15px; 
            border-radius: 8px; 
            font-size: 16px;
        }
        .success { 
            background: #d4edda; 
            color: #155724; 
            border: 1px solid #c3e6cb; 
        }
        .error { 
            background: #f8d7da; 
            color: #721c24; 
            border: 1px solid #f5c6cb; 
        }
        .extra-field { 
            margin-top: 20px; 
            animation: fadeIn 0.5s;
        }
        .back-link { 
            text-align: center; 
            margin-top: 30px; 
        }
        .back-link a { 
            color: #667eea; 
            text-decoration: none; 
            font-weight: bold;
            display: inline-block;
            padding: 10px 20px;
            border-radius: 6px;
            transition: background-color 0.3s;
        }
        .back-link a:hover { 
            text-decoration: underline; 
            background-color: #f5f5f5;
        }
        @keyframes fadeIn {
            from { opacity: 0; transform: translateY(-10px); }
            to { opacity: 1; transform: translateY(0); }
        }
        .required::after {
            content: " *";
            color: #e74c3c;
        }
        .form-group {
            margin-bottom: 20px;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>📝 Cadastrar Novo Funcionário</h1>
        
        <form id="cadastroForm">
            <div class="form-group">
                <label for="nome" class="required">Nome:</label>
                <input type="text" id="nome" name="nome" placeholder="Digite o nome completo" required>
            </div>
            
            <div class="form-group">
                <label for="salario" class="required">Salário (R$):</label>
                <input type="number" id="salario" name="salario" step="0.01" min="0" placeholder="Ex: 1500.00" required>
            </div>
            
            <div class="form-group">
                <label for="tipo" class="required">Tipo de Funcionário:</label>
                <select id="tipo" name="tipo" required>
                    <option value="">Selecione um tipo</option>
                    <option value="Vendedor">Vendedor</option>
                    <option value="Gerente">Gerente</option>
                    <option value="Caixa">Caixa</option>
                    <option value="Balconista">Balconista</option>
                </select>
            </div>
            
            <div id="camposExtras" class="extra-field">
                <!-- Campos extras aparecerão aqui -->
            </div>
            
            <button type="submit">➕ Cadastrar Funcionário</button>
        </form>
        
        <div id="resultado"></div>
        
        <div class="back-link">
            <a href="/">← Voltar para a página inicial</a>
        </div>
    </div>
    
    <script>
        // Mostrar campos extras baseado no tipo
        document.getElementById('tipo').addEventListener('change', function() {
            const camposExtras = document.getElementById('camposExtras');
            const tipo = this.value;
            
            let htmlExtra = '';
            
            if (tipo === 'Vendedor') {
                htmlExtra = `
                    <div class="form-group">
                        <label for="comissao">Comissão (R$):</label>
                        <input type="number" id="comissao" name="comissao" step="0.01" min="0" value="0" placeholder="Ex: 50.00">
                    </div>
                `;
            } else if (tipo === 'Gerente') {
                htmlExtra = `
                    <div class="form-group">
                        <label for="bonus">Bônus (R$):</label>
                        <input type="number" id="bonus" name="bonus" step="0.01" min="0" value="0" placeholder="Ex: 500.00">
                    </div>
                `;
            } else if (tipo === 'Caixa') {
                htmlExtra = `
                    <div class="form-group">
                        <label for="numeroCaixa">Número do Caixa:</label>
                        <input type="number" id="numeroCaixa" name="numeroCaixa" min="1" value="1">
                    </div>
                `;
            } else if (tipo === 'Balconista') {
                htmlExtra = `
                    <div class="form-group">
                        <label for="setor">Setor:</label>
                        <input type="text" id="setor" name="setor" placeholder="Ex: Padaria, Hortifruti...">
                    </div>
                `;
            }
            
            camposExtras.innerHTML = htmlExtra;
        });
        
        // Enviar formulário
        document.getElementById('cadastroForm').addEventListener('submit', async function(e) {
            e.preventDefault();
            
            const button = this.querySelector('button[type="submit"]');
            const originalText = button.textContent;
            button.textContent = 'Cadastrando...';
            button.disabled = true;
            
            const resultadoDiv = document.getElementById('resultado');
            resultadoDiv.innerHTML = '';
            resultadoDiv.className = '';
            
            // Validar dados
            const nome = document.getElementById('nome').value.trim();
            const salario = document.getElementById('salario').value;
            const tipo = document.getElementById('tipo').value;
            
            if (!nome || !salario || !tipo) {
                resultadoDiv.className = 'error';
                resultadoDiv.innerHTML = '<p>❌ Preencha todos os campos obrigatórios!</p>';
                button.textContent = originalText;
                button.disabled = false;
                return;
            }
            
            // Construir objeto JSON
            const data = {
                nome: nome,
                salario: parseFloat(salario),
                tipo: tipo
            };
            
            // Adicionar campos específicos
            if (tipo === 'Vendedor') {
                const comissao = document.getElementById('comissao')?.value;
                if (comissao) data.comissao = parseFloat(comissao);
            } else if (tipo === 'Gerente') {
                const bonus = document.getElementById('bonus')?.value;
                if (bonus) data.bonus = parseFloat(bonus);
            } else if (tipo === 'Caixa') {
                const numCaixa = document.getElementById('numeroCaixa')?.value;
                if (numCaixa) data.numeroCaixa = parseInt(numCaixa);
            } else if (tipo === 'Balconista') {
                const setor = document.getElementById('setor')?.value;
                if (setor) data.setor = setor.trim();
            }
            
            try {
                const response = await fetch('/api/funcionarios', {
                    method: 'POST',
                    headers: { 
                        'Content-Type': 'application/json',
                        'Accept': 'application/json'
                    },
                    body: JSON.stringify(data)
                });
                
                const result = await response.json();
                
                if (result.success) {
                    resultadoDiv.className = 'success';
                    resultadoDiv.innerHTML = `
                        <h3>✅ Funcionário cadastrado com sucesso!</h3>
                        <p><strong>Nome:</strong> ${result.nome}</p>
                        <p><strong>Tipo:</strong> ${result.tipo}</p>
                        <p><strong>ID:</strong> ${result.id}</p>
                        <p><strong>Salário:</strong> R$ ${result.salario}</p>
                        ${result.comissao ? `<p><strong>Comissão:</strong> R$ ${result.comissao}</p>` : ''}
                        ${result.bonus ? `<p><strong>Bônus:</strong> R$ ${result.bonus}</p>` : ''}
                        ${result.numeroCaixa ? `<p><strong>Número do Caixa:</strong> ${result.numeroCaixa}</p>` : ''}
                        ${result.setor ? `<p><strong>Setor:</strong> ${result.setor}</p>` : ''}
                        <p>Total de funcionários: ${result.total_funcionarios}</p>
                    `;
                    
                    // Limpar formulário
                    this.reset();
                    document.getElementById('camposExtras').innerHTML = '';
                } else {
                    resultadoDiv.className = 'error';
                    let mensagemErro = `<h3>❌ Erro ao cadastrar!</h3>`;
                    mensagemErro += `<p><strong>Motivo:</strong> ${result.message}</p>`;
                    if (result.dicas) {
                        mensagemErro += `<p><strong>Dicas:</strong></p><ul>`;
                        result.dicas.forEach(dica => {
                            mensagemErro += `<li>${dica}</li>`;
                        });
                        mensagemErro += `</ul>`;
                    }
                    resultadoDiv.innerHTML = mensagemErro;
                }
            } catch (error) {
                resultadoDiv.className = 'error';
                resultadoDiv.innerHTML = `
                    <h3>❌ Erro de conexão!</h3>
                    <p><strong>Detalhes:</strong> ${error.message}</p>
                    <p>Verifique se o servidor está rodando.</p>
                `;
                console.error('Erro:', error);
            } finally {
                button.textContent = originalText;
                button.disabled = false;
            }
        });
    </script>
</body>
</html>)";
        
        res.set_header("Content-Type", "text/html; charset=utf-8");
        res.set_content(html, "text/html");
    });
    
    // GET / - Página inicial MELHORADA
    svr.Get("/", [](const Request& req, Response& res) {
        cout << "🏠 GET / - Página inicial" << endl;
        
        auto funcionarios = supermercado.getFuncionarios();
        double totalFolha = 0;
        for (auto& f : funcionarios) totalFolha += f->getSalario();
        
        string html = R"(<!DOCTYPE html>
<html lang="pt-BR">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Supermercado API - Trabalho 3</title>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        
        body { 
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; 
            background: linear-gradient(135deg, #f5f7fa 0%, #c3cfe2 100%);
            color: #333;
            line-height: 1.6;
            min-height: 100vh;
            padding: 20px;
        }
        
        .container { 
            max-width: 1200px; 
            margin: 0 auto;
        }
        
        .header {
            background: linear-gradient(to right, #2c3e50, #4a6491);
            color: white;
            padding: 40px;
            border-radius: 15px;
            margin-bottom: 30px;
            box-shadow: 0 10px 30px rgba(0,0,0,0.1);
            text-align: center;
        }
        
        .header h1 {
            font-size: 2.8rem;
            margin-bottom: 10px;
            display: flex;
            align-items: center;
            justify-content: center;
            gap: 15px;
        }
        
        .header p {
            font-size: 1.2rem;
            opacity: 0.9;
            max-width: 800px;
            margin: 0 auto;
        }
        
        .main-content {
            display: grid;
            grid-template-columns: 1fr 1fr;
            gap: 30px;
            margin-bottom: 40px;
        }
        
        @media (max-width: 768px) {
            .main-content {
                grid-template-columns: 1fr;
            }
        }
        
        .card {
            background: white;
            border-radius: 15px;
            padding: 30px;
            box-shadow: 0 5px 20px rgba(0,0,0,0.08);
            transition: transform 0.3s, box-shadow 0.3s;
        }
        
        .card:hover {
            transform: translateY(-5px);
            box-shadow: 0 10px 30px rgba(0,0,0,0.15);
        }
        
        .card h2 {
            color: #2c3e50;
            border-bottom: 3px solid #3498db;
            padding-bottom: 15px;
            margin-bottom: 25px;
            display: flex;
            align-items: center;
            gap: 10px;
        }
        
        .card h2 i {
            font-size: 1.5em;
        }
        
        .stats-grid {
            display: grid;
            grid-template-columns: repeat(2, 1fr);
            gap: 20px;
            margin-top: 20px;
        }
        
        .stat-item {
            background: #f8f9fa;
            padding: 20px;
            border-radius: 10px;
            text-align: center;
        }
        
        .stat-value {
            font-size: 2.2rem;
            font-weight: bold;
            color: #2c3e50;
            margin: 10px 0;
        }
        
        .stat-label {
            color: #666;
            font-size: 0.9rem;
            text-transform: uppercase;
            letter-spacing: 1px;
        }
        
        .funcionarios-table {
            width: 100%;
            border-collapse: collapse;
            margin-top: 20px;
        }
        
        .funcionarios-table th {
            background: #2c3e50;
            color: white;
            padding: 15px;
            text-align: left;
            font-weight: 600;
        }
        
        .funcionarios-table td {
            padding: 15px;
            border-bottom: 1px solid #eee;
        }
        
        .funcionarios-table tr:hover {
            background: #f8f9fa;
        }
        
        .funcionarios-table .tipo {
            display: inline-block;
            padding: 5px 12px;
            border-radius: 20px;
            font-size: 0.85rem;
            font-weight: 600;
        }
        
        .tipo-vendedor { background: #e3f2fd; color: #1565c0; }
        .tipo-gerente { background: #e8f5e9; color: #2e7d32; }
        .tipo-caixa { background: #fff3e0; color: #ef6c00; }
        .tipo-balconista { background: #f3e5f5; color: #7b1fa2; }
        
        .btn {
            display: inline-block;
            background: linear-gradient(to right, #3498db, #2980b9);
            color: white;
            padding: 14px 28px;
            text-decoration: none;
            border-radius: 8px;
            font-weight: 600;
            font-size: 1rem;
            transition: all 0.3s;
            border: none;
            cursor: pointer;
        }
        
        .btn:hover {
            transform: translateY(-2px);
            box-shadow: 0 7px 14px rgba(52, 152, 219, 0.3);
        }
        
        .btn-success {
            background: linear-gradient(to right, #2ecc71, #27ae60);
        }
        
        .btn-success:hover {
            box-shadow: 0 7px 14px rgba(46, 204, 113, 0.3);
        }
        
        .btn-large {
            padding: 18px 40px;
            font-size: 1.2rem;
            display: flex;
            align-items: center;
            justify-content: center;
            gap: 10px;
        }
        
        .actions {
            display: flex;
            gap: 15px;
            margin-top: 30px;
            flex-wrap: wrap;
        }
        
        .endpoints {
            margin-top: 30px;
        }
        
        .endpoint-item {
            background: #f8f9fa;
            padding: 20px;
            margin-bottom: 15px;
            border-left: 5px solid #3498db;
            border-radius: 8px;
        }
        
        .method {
            display: inline-block;
            padding: 5px 12px;
            background: #3498db;
            color: white;
            border-radius: 5px;
            font-family: monospace;
            font-weight: bold;
            margin-right: 10px;
        }
        
        .method.get { background: #2ecc71; }
        .method.post { background: #e74c3c; }
        
        .url {
            font-family: monospace;
            background: #2c3e50;
            color: white;
            padding: 8px 12px;
            border-radius: 5px;
            display: block;
            margin-top: 10px;
            overflow-x: auto;
        }
        
        .footer {
            text-align: center;
            margin-top: 50px;
            padding-top: 30px;
            border-top: 2px solid #eee;
            color: #666;
        }
        
        .status-online {
            display: inline-block;
            width: 12px;
            height: 12px;
            background-color: #2ecc71;
            border-radius: 50%;
            animation: pulse 2s infinite;
        }
        
        @keyframes pulse {
            0% { opacity: 1; }
            50% { opacity: 0.5; }
            100% { opacity: 1; }
        }
        
        .loading {
            text-align: center;
            padding: 40px;
            color: #666;
        }
        
        .api-link {
            color: #3498db;
            text-decoration: none;
            font-weight: 500;
        }
        
        .api-link:hover {
            text-decoration: underline;
        }
        
        .salario {
            font-weight: 600;
            color: #27ae60;
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>🛒 Supermercado API REST</h1>
            <p><strong>Trabalho 3 - Sistemas Distribuídos</strong> | UFC Campus Quixadá</p>
        </div>
        
        <div class="actions">
            <a href="/cadastro" class="btn btn-success btn-large">
                <span>➕</span> Cadastrar Novo Funcionário
            </a>
            <a href="/api/funcionarios" class="btn" target="_blank">
                📋 Ver Todos os Funcionários (JSON)
            </a>
            <a href="/api/funcionarios/tipos" class="btn" target="_blank">
                📊 Ver Tipos de Funcionários
            </a>
        </div>
        
        <div class="main-content">
            <div class="card">
                <h2>📊 Estatísticas do Sistema</h2>
                <div id="stats">
                    <div class="loading">
                        <p>Carregando dados da API...</p>
                    </div>
                </div>
                
                <div class="stats-grid">
                    <div class="stat-item">
                        <div class="stat-value" id="total-func">0</div>
                        <div class="stat-label">Funcionários</div>
                    </div>
                    <div class="stat-item">
                        <div class="stat-value" id="total-folha">R$ 0.00</div>
                        <div class="stat-label">Folha de Pagamento</div>
                    </div>
                </div>
            </div>
            
            <div class="card">
                <h2>👥 Funcionários Cadastrados</h2>
                <div id="funcionarios-lista">
                    <div class="loading">
                        <p>Carregando lista de funcionários...</p>
                    </div>
                </div>
            </div>
        </div>
        
        <div class="card">
            <h2>🔧 Endpoints da API</h2>
            <div class="endpoints">
                <div class="endpoint-item">
                    <span class="method get">GET</span>
                    <strong>/api/funcionarios</strong>
                    <p>Listar todos os funcionários do supermercado</p>
                    <code class="url">http://)";
        
        // Adiciona o IP atual para acesso em rede
    html += "172.25.183.184";        
        html += R"(/api/funcionarios</code>
                </div>
                
                <div class="endpoint-item">
                    <span class="method get">GET</span>
                    <strong>/api/funcionarios/folha</strong>
                    <p>Calcular folha de pagamento total</p>
                    <code class="url">http://)";
        html += "172.25.183.184";
        html += R"(/api/funcionarios/folha</code>
                </div>
                
                <div class="endpoint-item">
                    <span class="method get">GET</span>
                    <strong>/api/funcionarios/tipos</strong>
                    <p>Listar todos os tipos de funcionários disponíveis</p>
                    <code class="url">http://)";
        html += "172.25.183.184";
        html += R"(/api/funcionarios/tipos</code>
                </div>
                
                <div class="endpoint-item">
                    <span class="method get">GET</span>
                    <strong>/api/funcionarios/tipo/{tipo}</strong>
                    <p>Filtrar funcionários por tipo (Vendedor, Gerente, Caixa, Balconista)</p>
                    <code class="url">http://)";
        html += "172.25.183.184";
        html += R"(/api/funcionarios/tipo/Vendedor</code>
                </div>
                
                <div class="endpoint-item">
                    <span class="method post">POST</span>
                    <strong>/api/funcionarios</strong>
                    <p>Adicionar novo funcionário (enviar JSON no corpo)</p>
                    <code class="url">curl -X POST -H "Content-Type: application/json" -d '{"nome":"Nome","salario":2000,"tipo":"Vendedor"}' http://)";
        html += "172.25.183.184";       
        html += R"(/api/funcionarios</code>
html += R"(/api/funcionarios</code>
                </div>
            </div>
        </div>
        
        <div class="card">
            <h2>🌐 Acesso em Rede</h2>
            <p>Para acessar este servidor de outros computadores:</p>
            <ol style="margin: 15px 0 15px 20px;">
                <li>Verifique seu IP local (ifconfig no Linux, ipconfig no Windows)</li>
                <li>Substitua "localhost" pelo IP do servidor</li>
                <li>Certifique-se de que o firewall permite conexões na porta 8080</li>
            </ol>
            <p><strong>URLs para acesso em rede:</strong></p>
            <ul style="margin: 15px 0 15px 20px;">
                <li>Página inicial: <code>http://<span id="server-ip">SEU_IP</span>:8080</code></li>
                <li>API Funcionários: <code>http://<span id="server-ip-api">SEU_IP</span>:8080/api/funcionarios</code></li>
                <li>Formulário: <code>http://<span id="server-ip-form">SEU_IP</span>:8080/cadastro</code></li>
            </ul>
        </div>
        
        <div class="footer">
            <p>
                <span class="status-online"></span>
                <strong>Status:</strong> <span id="status-text">Online</span> |
                <strong>Servidor:</strong> <span id="server-url">http://localhost:8080</span> |
                <strong>Atualizado:</strong> <span id="last-update">Agora</span>
            </p>
            <p style="margin-top: 10px; font-size: 0.9rem;">
                Trabalho 3 - Sistemas Distribuídos | UFC Quixadá | Desenvolvido com C++ & httplib
            </p>
        </div>
    </div>
    
    <script>
        // Função para obter o IP do servidor
        async function obterIP() {
            try {
                const response = await fetch('https://api.ipify.org?format=json');
                const data = await response.json();
                return data.ip;
            } catch {
                return window.location.hostname;
            }
        }
        
        // Atualizar estatísticas e lista de funcionários
        async function atualizarDados() {
            try {
                // Atualizar estatísticas
                const response = await fetch('/api/funcionarios');
                if (!response.ok) throw new Error('Erro na requisição');
                
                const funcionarios = await response.json();
                const totalFunc = funcionarios.length;
                const totalFolha = funcionarios.reduce((sum, f) => sum + f.salario, 0);
                
                // Atualizar contadores
                document.getElementById('total-func').textContent = totalFunc;
                document.getElementById('total-folha').textContent = `R$ ${totalFolha.toFixed(2)}`;
                
                // Atualizar stats card
                document.getElementById('stats').innerHTML = `
                    <p>O sistema possui <strong>${totalFunc} funcionários</strong> cadastrados.</p>
                    <p>A folha de pagamento mensal totaliza <strong>R$ ${totalFolha.toFixed(2)}</strong>.</p>
                    <p>A média salarial é de <strong>R$ ${(totalFolha / (totalFunc || 1)).toFixed(2)}</strong> por funcionário.</p>
                `;
                
                // Atualizar lista de funcionários
                let htmlLista = '';
                if (funcionarios.length > 0) {
                    htmlLista = `
                        <table class="funcionarios-table">
                            <thead>
                                <tr>
                                    <th>ID</th>
                                    <th>Nome</th>
                                    <th>Tipo</th>
                                    <th>Salário</th>
                                </tr>
                            </thead>
                            <tbody>
                    `;
                    
                    funcionarios.forEach(func => {
                        const tipoClass = `tipo-${func.tipo.toLowerCase()}`;
                        htmlLista += `
                            <tr>
                                <td>${func.id}</td>
                                <td>${func.nome}</td>
                                <td><span class="tipo ${tipoClass}">${func.tipo}</span></td>
                                <td class="salario">R$ ${func.salario.toFixed(2)}</td>
                            </tr>
                        `;
                    });
                    
                    htmlLista += `
                            </tbody>
                        </table>
                        <p style="margin-top: 15px; color: #666;">
                            Mostrando ${funcionarios.length} funcionário${funcionarios.length !== 1 ? 's' : ''}
                        </p>
                    `;
                } else {
                    htmlLista = '<p>Nenhum funcionário cadastrado ainda.</p>';
                }
                
                document.getElementById('funcionarios-lista').innerHTML = htmlLista;
                
                // Atualizar timestamp
                const agora = new Date();
                document.getElementById('last-update').textContent = 
                    agora.toLocaleTimeString('pt-BR', { hour: '2-digit', minute: '2-digit', second: '2-digit' });
                
            } catch (error) {
                console.error('Erro ao atualizar dados:', error);
                document.getElementById('stats').innerHTML = 
                    '<p style="color: #e74c3c;">Erro ao carregar dados da API.</p>';
                document.getElementById('funcionarios-lista').innerHTML = 
                    '<p style="color: #e74c3c;">Erro ao carregar lista de funcionários.</p>';
            }
        }
        
        // Inicializar
        document.addEventListener('DOMContentLoaded', async () => {
            // Atualizar IPs na página
            const serverIP = await obterIP() || 'localhost';
            const elements = document.querySelectorAll('#server-ip, #server-ip-api, #server-ip-form');
            elements.forEach(el => {
                el.textContent = serverIP;
            });
            
            // Atualizar URLs
            const serverUrl = `http://${serverIP}:8080`;
            document.getElementById('server-url').textContent = serverUrl;
            
            // Atualizar dados inicialmente
            await atualizarDados();
            
            // Atualizar a cada 5 segundos
            setInterval(atualizarDados, 5000);
        });
    </script>
</body>
</html>)";
        
        res.set_header("Content-Type", "text/html; charset=utf-8");
        res.set_content(html, "text/html");
    });
    
    // ==================== CONFIGURAÇÃO DO SERVIDOR ====================
    
    // CORS para navegadores
    svr.Options(R"(.*)", [](const Request& req, Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        res.status = 200;
    });
    
    // Tratamento de erros 404
    svr.set_error_handler([](const Request& req, Response& res) {
        string html = R"(<!DOCTYPE html>
<html lang="pt-BR">
<head>
    <meta charset="UTF-8">
    <title>Página não encontrada - Supermercado API</title>
    <style>
        body { 
            font-family: 'Segoe UI', sans-serif; 
            text-align: center; 
            padding: 100px 20px;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
            min-height: 100vh;
        }
        h1 { 
            font-size: 5rem; 
            margin-bottom: 20px;
            opacity: 0.9;
        }
        p { 
            font-size: 1.2rem; 
            margin-bottom: 30px;
            opacity: 0.8;
        }
        a { 
            color: white; 
            text-decoration: none;
            border: 2px solid white;
            padding: 12px 30px;
            border-radius: 50px;
            font-weight: bold;
            transition: all 0.3s;
            display: inline-block;
        }
        a:hover { 
            background: white;
            color: #667eea;
        }
    </style>
</head>
<body>
    <h1>404</h1>
    <p>A página que você está procurando não existe.</p>
    <a href="/">🏠 Voltar para a página inicial</a>
</body>
</html>)";
        
        res.set_header("Content-Type", "text/html; charset=utf-8");
        res.set_content(html, "text/html");
    });
    
    // Configurações do servidor
    svr.set_keep_alive_max_count(500);
    svr.set_read_timeout(10, 0); // 10 segundos
    svr.set_write_timeout(10, 0); // 10 segundos
    
    cout << "\n✅ SERVIDOR CONFIGURADO COM SUCESSO!" << endl;
    cout << "\n=== INFORMAÇÕES DE ACESSO ===" << endl;
    cout << "📍 Local: http://localhost:8080" << endl;
    cout << "🌐 Rede WiFi: http://172.25.183.184:8080" << endl;
    cout << "🌐 Rede Cabo: http://10.0.109.101:8080" << endl;
    cout << "  GET  /                    Página inicial" << endl;
    cout << "  GET  /cadastro            Formulário de cadastro" << endl;
    cout << "  GET  /api/funcionarios    Listar todos os funcionários" << endl;
    cout << "  GET  /api/funcionarios/folha  Calcular folha de pagamento" << endl;
    cout << "  GET  /api/funcionarios/tipos  Listar tipos de funcionários" << endl;
    cout << "  GET  /api/funcionarios/tipo/{tipo}  Filtrar por tipo" << endl;
    cout << "  POST /api/funcionarios    Adicionar novo funcionário" << endl;
    cout << "\n=== INSTRUÇÕES PARA ACESSO EM REDE ===" << endl;
    cout << "1. Descubra seu IP local: ifconfig ou ipconfig" << endl;
    cout << "2. Em outro computador, acesse: http://SEU_IP:8080" << endl;
    cout << "3. Para clientes C++/Python, use o IP em vez de localhost" << endl;
    cout << "\n⏳ Aguardando conexões..." << endl;
    cout << "Pressione Ctrl+C para encerrar" << endl;
    cout << "==================================================" << endl;
    
    // Iniciar servidor em TODAS as interfaces de rede
    if (!svr.listen("0.0.0.0", 8080)) {
        cerr << "❌ ERRO: Não foi possível iniciar o servidor!" << endl;
        cerr << "Verifique se a porta 8080 está disponível." << endl;
        return 1;
    }
    
    return 0;
}