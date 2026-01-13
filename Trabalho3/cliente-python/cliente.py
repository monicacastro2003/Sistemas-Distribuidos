import requests
import json

class SupermercadoCliente:
    def __init__(self, server_ip="localhost"):
        self.base_url = f"http://{server_ip}:8080"
        print(f"🔗 Conectando ao servidor: {self.base_url}")
    
    def testar_conexao(self):
        """Testar se consegue conectar ao servidor"""
        try:
            response = requests.get(f"{self.base_url}/", timeout=5)
            return response.status_code == 200
        except:
            return False
    
    def fazer_requisicao(self, metodo, endpoint, dados=None):
        """Fazer requisição HTTP"""
        url = f"{self.base_url}{endpoint}"
        
        try:
            if metodo == "GET":
                response = requests.get(url, timeout=10)
            elif metodo == "POST":
                headers = {"Content-Type": "application/json"}
                response = requests.post(url, json=dados, headers=headers, timeout=10)
            
            response.raise_for_status()
            return response.json()
        except requests.exceptions.ConnectionError:
            print(f"❌ Não foi possível conectar ao servidor: {self.base_url}")
            print("   Verifique:")
            print("   1. O servidor está rodando?")
            print("   2. O IP está correto?")
            print("   3. Ambos estão na mesma rede?")
            return None
        except Exception as e:
            print(f"❌ Erro: {e}")
            return None

def mostrar_menu():
    print("\n" + "="*50)
    print("🐍 CLIENTE PYTHON - SUPERMERCADO API")
    print("="*50)
    print("1. 📋 Listar todos os funcionários")
    print("2. 💰 Calcular folha de pagamento")
    print("3. 📊 Listar tipos de funcionários")
    print("4. 🔍 Filtrar por tipo")
    print("5. ➕ Cadastrar novo funcionário")
    print("6. 🌐 Testar conexão")
    print("7. 🚪 Sair")
    print("="*50)

def main():
    print("=== CONFIGURAÇÃO DE REDE ===")
    server_ip = input("Digite o IP do servidor (ou Enter para localhost): ").strip()
    if not server_ip:
        server_ip = "localhost"
    
    cliente = SupermercadoCliente(server_ip)
    
    # Testar conexão
    print(f"\n🔍 Testando conexão com {server_ip}...")
    if not cliente.testar_conexao():
        print("❌ Não foi possível conectar ao servidor!")
        print("   Verifique o IP e se o servidor está rodando.")
        return
    
    print("✅ Conexão estabelecida com sucesso!")
    
    while True:
        mostrar_menu()
        opcao = input("\nEscolha uma opção: ").strip()
        
        if opcao == "7":
            print("\n👋 Encerrando cliente...")
            break
        elif opcao == "6":
            if cliente.testar_conexao():
                print("✅ Servidor está respondendo!")
            else:
                print("❌ Servidor não está respondendo!")
            input("\nPressione Enter para continuar...")
            continue
        
        resultado = None
        
        if opcao == "1":
            print("\n📋 Listando funcionários...")
            resultado = cliente.fazer_requisicao("GET", "/api/funcionarios")
            if resultado:
                print(f"\n✅ Total: {len(resultado)} funcionários")
                for func in resultado:
                    print(f"\n  ID: {func.get('id')}")
                    print(f"  Nome: {func.get('nome')}")
                    print(f"  Tipo: {func.get('tipo')}")
                    print(f"  Salário: R$ {func.get('salario'):.2f}")
        
        elif opcao == "2":
            print("\n💰 Calculando folha...")
            resultado = cliente.fazer_requisicao("GET", "/api/funcionarios/folha")
            if resultado:
                print(f"\n✅ Folha total: R$ {resultado.get('folha_pagamento'):.2f}")
                print(f"✅ Funcionários: {resultado.get('quantidade_funcionarios')}")
                print(f"✅ Média salarial: R$ {resultado.get('media_salarios'):.2f}")
        
        elif opcao == "3":
            print("\n📊 Listando tipos...")
            resultado = cliente.fazer_requisicao("GET", "/api/funcionarios/tipos")
            if resultado:
                print("\n✅ Tipos disponíveis:")
                for tipo in resultado.get('tipos_disponiveis', []):
                    print(f"  • {tipo}")
        
        elif opcao == "4":
            tipo = input("\nDigite o tipo (Vendedor/Gerente/Caixa/Balconista): ").strip()
            print(f"\n🔍 Filtrando por {tipo}...")
            resultado = cliente.fazer_requisicao("GET", f"/api/funcionarios/tipo/{tipo}")
            if resultado:
                print(f"\n✅ Tipo: {resultado.get('tipo')}")
                print(f"✅ Quantidade: {resultado.get('quantidade')}")
                for func in resultado.get('funcionarios', []):
                    print(f"\n  ID: {func.get('id')}")
                    print(f"  Nome: {func.get('nome')}")
                    print(f"  Salário: R$ {func.get('salario'):.2f}")
        
        elif opcao == "5":
            print("\n➕ CADASTRAR NOVO FUNCIONÁRIO")
            print("="*30)
            
            dados = {}
            dados["nome"] = input("Nome: ").strip()
            
            while True:
                try:
                    salario = input("Salário (R$): ").replace(',', '.')
                    dados["salario"] = float(salario)
                    break
                except ValueError:
                    print("❌ Digite um número válido")
            
            tipos_validos = ["Vendedor", "Gerente", "Caixa", "Balconista"]
            while True:
                tipo = input("Tipo (Vendedor/Gerente/Caixa/Balconista): ").strip()
                if tipo in tipos_validos:
                    dados["tipo"] = tipo
                    break
                else:
                    print(f"❌ Tipo inválido. Escolha entre: {', '.join(tipos_validos)}")
            
            tipo = dados["tipo"]
            if tipo == "Vendedor":
                while True:
                    try:
                        comissao = input("Comissão (R$): ").replace(',', '.')
                        dados["comissao"] = float(comissao)
                        break
                    except ValueError:
                        print("❌ Digite um número válido")
            elif tipo == "Gerente":
                while True:
                    try:
                        bonus = input("Bônus (R$): ").replace(',', '.')
                        dados["bonus"] = float(bonus)
                        break
                    except ValueError:
                        print("❌ Digite um número válido")
            elif tipo == "Caixa":
                while True:
                    try:
                        dados["numeroCaixa"] = int(input("Número do caixa: "))
                        break
                    except ValueError:
                        print("❌ Digite um número inteiro")
            elif tipo == "Balconista":
                dados["setor"] = input("Setor: ").strip()
            
            print(f"\n📦 Enviando dados...")
            resultado = cliente.fazer_requisicao("POST", "/api/funcionarios", dados)
            
            if resultado:
                if resultado.get('success'):
                    print(f"\n🎉 {resultado.get('message')}")
                    print(f"   🆔 ID: {resultado.get('id')}")
                    print(f"   👤 Nome: {resultado.get('nome')}")
                    print(f"   📋 Tipo: {resultado.get('tipo')}")
                else:
                    print(f"\n❌ {resultado.get('message')}")
        
        else:
            print("\n❌ Opção inválida!")
        
        if opcao not in ["6", "7"]:
            input("\n⏎ Pressione Enter para continuar...")

if __name__ == "__main__":
    main()