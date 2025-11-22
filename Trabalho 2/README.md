# Trabalho 2 – RMI 

Este projeto implementa um sistema cliente-servidor usando **Java RMI**, seguindo o protocolo de **requisição–resposta**.
O cliente envia uma requisição contendo o nome do objeto remoto, o método a ser chamado e os argumentos. O servidor processa essa requisição, executa o método e devolve uma resposta empacotada no mesmo formato.

A aplicação contém:
- Classes de entidades com relações de herança e agregação;
- Quatro métodos remotos acessíveis via RMI;
- Passagem de parâmetros por referência (para objetos remotos) e por valor (para objetos locais), usando representação externa como JSON/XML/Protocol Buffers.

O cliente chama métodos como se estivessem locais, mas a execução ocorre no servidor através do RMI.


### 🎥 Vídeo explicativo
[Assista ao vídeo no Google Drive](https://drive.google.com/file/d/1gc1p96SjnSP8nJgridQfGm3jmQApKPgJ/view?usp=drivesdk)

obs: Contém dois vídeos juntos. Um explicando o Trabalho 2 e um mais curto falando sobre o Sistema de Votação do Trabalho 1.
