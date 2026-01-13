#include <iostream>
#include <curl/curl.h>
#include <string>
#include <sstream>
#include <cstring>

using namespace std;

size_t WriteCallback(void* contents, size_t size, size_t nmemb, string* output) {
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);
    return totalSize;
}

bool testarConexao(const string& server_url) {
    CURL* curl = curl_easy_init();
    if(!curl) return false;
    
    string url = server_url + "/api/funcionarios";
    string response;
    
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);
    
    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    
    return res == CURLE_OK;
}

int main() {
    CURL* curl;
    CURLcode res;
    string response;
    
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    
    if(curl) {
        cout << "======================================" << endl;
        cout << "CLIENTE C++ - SUPERMERCADO API" << endl;
        cout << "======================================" << endl;
        
        // Configuração de rede
        cout << "\n=== CONFIGURAÇÃO DE REDE ===" << endl;
        cout << "Digite o IP do servidor (ou Enter para localhost): ";
        
        string server_ip;
        getline(cin, server_ip);
        if(server_ip.empty()) {
            server_ip = "localhost";
        }
        
        string server_url = "http://" + server_ip + ":8080";
        
        cout << "\n🔍 Testando conexão com " << server_url << "..." << endl;
        
        if(!testarConexao(server_url)) {
            cout << "❌ Não foi possível conectar ao servidor!" << endl;
            cout << "   Verifique:" << endl;
            cout << "   1. O servidor está rodando?" << endl;
            cout << "   2. O IP está correto? (Seu IP: 172.25.183.184)" << endl;
            cout << "   3. Ambos estão na mesma rede?" << endl;
            cout << "   4. Firewall pode estar bloqueando" << endl;
            
            curl_easy_cleanup(curl);
            curl_global_cleanup();
            return 1;
        }
        
        cout << "✅ Conexão estabelecida com sucesso!" << endl;
        
        while(true) {
            cout << "\n📋 MENU PRINCIPAL:" << endl;
            cout << "1. Listar todos os funcionários" << endl;
            cout << "2. Calcular folha de pagamento" << endl;
            cout << "3. Listar tipos de funcionários" << endl;
            cout << "4. Filtrar por tipo" << endl;
            cout << "5. Cadastrar novo funcionário" << endl;
            cout << "6. Testar conexão" << endl;
            cout << "7. Sair" << endl;
            cout << "\nEscolha uma opção: ";
            
            int opcao;
            cin >> opcao;
            cin.ignore();
            
            if(opcao == 7) {
                cout << "\n👋 Encerrando cliente..." << endl;
                break;
            }
            
            if(opcao == 6) {
                if(testarConexao(server_url))
                    cout << "\n✅ Servidor está respondendo!" << endl;
                else
                    cout << "\n❌ Servidor não está respondendo!" << endl;
                cout << "\nPressione Enter para continuar...";
                cin.get();
                continue;
            }
            
            string url;
            string post_data;
            
            switch(opcao) {
                case 1:
                    url = server_url + "/api/funcionarios";
                    cout << "\n📋 Listando todos os funcionários..." << endl;
                    break;
                    
                case 2:
                    url = server_url + "/api/funcionarios/folha";
                    cout << "\n💰 Calculando folha de pagamento..." << endl;
                    break;
                    
                case 3:
                    url = server_url + "/api/funcionarios/tipos";
                    cout << "\n📊 Listando tipos de funcionários..." << endl;
                    break;
                    
                case 4:
                    {
                        cout << "\n🔍 Tipos disponíveis: Vendedor, Gerente, Caixa, Balconista" << endl;
                        cout << "Digite o tipo: ";
                        string tipo;
                        getline(cin, tipo);
                        url = server_url + "/api/funcionarios/tipo/" + tipo;
                        cout << "\n🔍 Filtrando por tipo: " << tipo << endl;
                    }
                    break;
                    
                case 5:
                    {
                        cout << "\n➕ CADASTRAR NOVO FUNCIONÁRIO" << endl;
                        cout << "=============================" << endl;
                        
                        string nome, tipo;
                        double salario;
                        
                        cout << "Nome: ";
                        getline(cin, nome);
                        
                        cout << "Salário (R$): ";
                        cin >> salario;
                        cin.ignore();
                        
                        cout << "Tipo (Vendedor/Gerente/Caixa/Balconista): ";
                        getline(cin, tipo);
                        
                        // Construir JSON CORRETAMENTE
                        stringstream json_stream;
                        json_stream << "{";
                        json_stream << "\"nome\":\"" << nome << "\",";
                        json_stream << "\"salario\":" << salario << ",";
                        json_stream << "\"tipo\":\"" << tipo << "\"";
                        
                        // Adicionar campos específicos
                        if(tipo == "Vendedor") {
                            double comissao;
                            cout << "Comissão (R$): ";
                            cin >> comissao;
                            cin.ignore();
                            json_stream << ",\"comissao\":" << comissao;
                        }
                        else if(tipo == "Gerente") {
                            double bonus;
                            cout << "Bônus (R$): ";
                            cin >> bonus;
                            cin.ignore();
                            json_stream << ",\"bonus\":" << bonus;
                        }
                        else if(tipo == "Caixa") {
                            int numCaixa;
                            cout << "Número do caixa: ";
                            cin >> numCaixa;
                            cin.ignore();
                            json_stream << ",\"numeroCaixa\":" << numCaixa;
                        }
                        else if(tipo == "Balconista") {
                            string setor;
                            cout << "Setor: ";
                            getline(cin, setor);
                            json_stream << ",\"setor\":\"" << setor << "\"";
                        }
                        
                        json_stream << "}";
                        post_data = json_stream.str();
                        url = server_url + "/api/funcionarios";
                        
                        cout << "\n📦 JSON enviado: " << post_data << endl;
                    }
                    break;
                    
                default:
                    cout << "\n❌ Opção inválida!" << endl;
                    continue;
            }
            
            // Configurar requisição
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
            
            // Se for POST, configurar dados
            if(opcao == 5) {
                curl_easy_setopt(curl, CURLOPT_POST, 1L);
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data.c_str());
                
                struct curl_slist* headers = NULL;
                headers = curl_slist_append(headers, "Content-Type: application/json");
                curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
            }
            
            // Executar requisição
            res = curl_easy_perform(curl);
            
            if(res != CURLE_OK) {
                cerr << "\n❌ Erro na requisição: " << curl_easy_strerror(res) << endl;
            } else {
                cout << "\n✅ Resposta do servidor:" << endl;
                cout << "========================" << endl;
                cout << response << endl;
                cout << "========================" << endl;
            }
            
            // Limpar resposta
            response.clear();
            
            if(opcao == 5) {
                // Limpar headers do POST
                struct curl_slist* headers = NULL;
                curl_slist_free_all(headers);
            }
            
            cout << "\nPressione Enter para continuar...";
            cin.get();
        }
        
        // Limpar
        curl_easy_cleanup(curl);
    }
    
    curl_global_cleanup();
    return 0;
}