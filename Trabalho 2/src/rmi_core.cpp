#include <cstring>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <thread>
#include <map>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "../include/rmi_core.h"  // DEVE SER O PRIMEIRO INCLUDE
#include "../include/rmi_protocol.h"
#include "../include/remote_service.h"
#include "../include/serializer.h"
#include "../include/subclasses.h"

using namespace std;

// ========== IMPLEMENTAÇÕES DAS CLASSES DE PROTOCOLO ==========

vector<char> RemoteObjectRef::serialize() const {
    string data = objectId + "|" + host + "|" + to_string(port);
    return vector<char>(data.begin(), data.end());
}

RemoteObjectRef RemoteObjectRef::deserialize(const vector<char>& data) {
    string str(data.begin(), data.end());
    size_t pos1 = str.find('|');
    size_t pos2 = str.find('|', pos1 + 1);
    
    if (pos1 == string::npos || pos2 == string::npos) {
        return RemoteObjectRef();
    }
    
    string objId = str.substr(0, pos1);
    string h = str.substr(pos1 + 1, pos2 - pos1 - 1);
    int p = stoi(str.substr(pos2 + 1));
    
    return RemoteObjectRef(objId, h, p);
}

vector<char> RMIMessage::serialize() const {
    ostringstream os(ios::binary);
    
    int32_t type = static_cast<int32_t>(messageType);
    os.write(reinterpret_cast<const char*>(&type), sizeof(type));
    
    int32_t reqId = htonl(requestId);
    os.write(reinterpret_cast<const char*>(&reqId), sizeof(reqId));
    
    auto objData = objectReference.serialize();
    int32_t objSize = htonl(objData.size());
    os.write(reinterpret_cast<const char*>(&objSize), sizeof(objSize));
    os.write(objData.data(), objData.size());
    
    int32_t methodSize = htonl(methodId.size());
    os.write(reinterpret_cast<const char*>(&methodSize), sizeof(methodSize));
    os.write(methodId.c_str(), methodId.size());
    
    int32_t argSize = htonl(arguments.size());
    os.write(reinterpret_cast<const char*>(&argSize), sizeof(argSize));
    os.write(arguments.data(), arguments.size());
    
    string str = os.str();
    return vector<char>(str.begin(), str.end());
}

RMIMessage RMIMessage::deserialize(const vector<char>& data) {
    RMIMessage msg;
    string str(data.begin(), data.end());
    istringstream is(str, ios::binary);
    
    int32_t type;
    is.read(reinterpret_cast<char*>(&type), sizeof(type));
    msg.messageType = static_cast<MessageType>(type);
    
    int32_t reqId;
    is.read(reinterpret_cast<char*>(&reqId), sizeof(reqId));
    msg.requestId = ntohl(reqId);
    
    int32_t objSize;
    is.read(reinterpret_cast<char*>(&objSize), sizeof(objSize));
    objSize = ntohl(objSize);
    vector<char> objData(objSize);
    is.read(objData.data(), objSize);
    msg.objectReference = RemoteObjectRef::deserialize(objData);
    
    int32_t methodSize;
    is.read(reinterpret_cast<char*>(&methodSize), sizeof(methodSize));
    methodSize = ntohl(methodSize);
    msg.methodId.resize(methodSize);
    is.read(&msg.methodId[0], methodSize);
    
    int32_t argSize;
    is.read(reinterpret_cast<char*>(&argSize), sizeof(argSize));
    argSize = ntohl(argSize);
    msg.arguments.resize(argSize);
    is.read(msg.arguments.data(), argSize);
    
    return msg;
}

// ========== IMPLEMENTAÇÕES DAS CLASSES RMI ==========

// RMIServer
RMIServer::RMIServer(int port) : serverPort(port), serverSocket(-1), nextRequestId(1) {}

bool RMIServer::start() {
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        perror("Erro ao criar socket do servidor");
        return false;
    }
    
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    
    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Erro no bind");
        close(serverSocket);
        return false;
    }
    
    if (listen(serverSocket, 5) < 0) {
        perror("Erro no listen");
        close(serverSocket);
        return false;
    }
    
    cout << "Servidor RMI ouvindo na porta " << serverPort << endl;
    return true;
}

void RMIServer::registerObject(const string& objectId, IRemoteSupermercadoService* service) {
    remoteObjects[objectId] = service;
}

void RMIServer::run() {
    while (true) {
        sockaddr_in clientAddr{};
        socklen_t clientLen = sizeof(clientAddr);
        int clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientLen);
        
        if (clientSocket < 0) {
            perror("Erro no accept");
            continue;
        }
        
        thread([this, clientSocket]() {
            this->handleClient(clientSocket);
        }).detach();
    }
}

RMIMessage RMIServer::getRequest(int clientSocket) {
    int32_t msgSize;
    if (recv_all(clientSocket, &msgSize, sizeof(msgSize)) <= 0) {
        throw runtime_error("Erro ao receber tamanho da mensagem");
    }
    msgSize = ntohl(msgSize);
    
    vector<char> msgData(msgSize);
    if (recv_all(clientSocket, msgData.data(), msgSize) <= 0) {
        throw runtime_error("Erro ao receber dados da mensagem");
    }
    
    return RMIMessage::deserialize(msgData);
}

void RMIServer::sendReply(const vector<char>& reply, int clientSocket) {
    int32_t msgSize = htonl(reply.size());
    if (send_all(clientSocket, &msgSize, sizeof(msgSize)) <= 0) {
        throw runtime_error("Erro ao enviar tamanho da resposta");
    }
    
    if (send_all(clientSocket, reply.data(), reply.size()) <= 0) {
        throw runtime_error("Erro ao enviar dados da resposta");
    }
}

void RMIServer::handleClient(int clientSocket) {
    try {
        RMIMessage request = getRequest(clientSocket);
        RMIMessage reply;
        reply.messageType = REPLY;
        reply.requestId = request.requestId;
        reply.objectReference = request.objectReference;
        reply.methodId = request.methodId;
        
        vector<char> result = executeMethod(request);
        reply.arguments = result;
        
        sendReply(reply.serialize(), clientSocket);
        
    } catch (const exception& e) {
        cerr << "Erro no handleClient: " << e.what() << endl;
    }
    close(clientSocket);
}

vector<char> RMIServer::executeMethod(const RMIMessage& request) {
    auto it = remoteObjects.find(request.objectReference.getObjectId());
    if (it == remoteObjects.end()) {
        throw runtime_error("Objeto remoto não encontrado: " + request.objectReference.getObjectId());
    }
    
    IRemoteSupermercadoService* service = it->second;
    
    if (request.methodId == "adicionarFuncionario") {
        bool success = service->adicionarFuncionario(request.arguments);
        return serializeBool(success);
    }
    else if (request.methodId == "listarFuncionarios") {
        return service->listarFuncionarios();
    }
    else if (request.methodId == "buscarFuncionarioPorId") {
        return service->buscarFuncionarioPorId(deserializeInt(request.arguments));
    }
    else if (request.methodId == "calcularFolhaPagamento") {
        double resultado = service->calcularFolhaPagamento();
        return serializeDouble(resultado);
    }
    else if (request.methodId == "getFuncionariosPorTipo") {
        return service->getFuncionariosPorTipo(deserializeString(request.arguments));
    }
    else {
        throw runtime_error("Método não implementado: " + request.methodId);
    }
}

vector<char> RMIServer::serializeBool(bool value) {
    vector<char> result(1);
    result[0] = value ? 1 : 0;
    return result;
}

vector<char> RMIServer::serializeInt(int value) {
    int32_t netValue = htonl(value);
    vector<char> result(sizeof(netValue));
    memcpy(result.data(), &netValue, sizeof(netValue));
    return result;
}

vector<char> RMIServer::serializeDouble(double value) {
    vector<char> result(sizeof(value));
    memcpy(result.data(), &value, sizeof(value));
    return result;
}

vector<char> RMIServer::serializeString(const string& str) {
    vector<char> result(str.begin(), str.end());
    return result;
}

int RMIServer::deserializeInt(const vector<char>& data) {
    int32_t value;
    memcpy(&value, data.data(), sizeof(value));
    return ntohl(value);
}

string RMIServer::deserializeString(const vector<char>& data) {
    return string(data.begin(), data.end());
}

// RMIClient
RMIClient::RMIClient(const string& host, int port) : serverHost(host), serverPort(port) {}

vector<char> RMIClient::doOperation(const RemoteObjectRef& remoteObject, 
                                   const string& methodId, 
                                   const vector<char>& arguments) {
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        throw runtime_error("Erro ao criar socket do cliente");
    }
    
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);
    inet_pton(AF_INET, serverHost.c_str(), &serverAddr.sin_addr);
    
    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        close(clientSocket);
        throw runtime_error("Erro ao conectar ao servidor");
    }
    
    RMIMessage request;
    request.messageType = REQUEST;
    request.requestId = 1;
    request.objectReference = remoteObject;
    request.methodId = methodId;
    request.arguments = arguments;
    
    vector<char> requestData = request.serialize();
    
    int32_t msgSize = htonl(requestData.size());
    if (send_all(clientSocket, &msgSize, sizeof(msgSize)) <= 0) {
        close(clientSocket);
        throw runtime_error("Erro ao enviar tamanho da requisição");
    }
    
    if (send_all(clientSocket, requestData.data(), requestData.size()) <= 0) {
        close(clientSocket);
        throw runtime_error("Erro ao enviar dados da requisição");
    }
    
    RMIMessage reply = receiveReply(clientSocket);
    close(clientSocket);
    
    return reply.arguments;
}

RMIMessage RMIClient::receiveReply(int clientSocket) {
    int32_t msgSize;
    if (recv_all(clientSocket, &msgSize, sizeof(msgSize)) <= 0) {
        throw runtime_error("Erro ao receber tamanho da resposta");
    }
    msgSize = ntohl(msgSize);
    
    vector<char> replyData(msgSize);
    if (recv_all(clientSocket, replyData.data(), msgSize) <= 0) {
        throw runtime_error("Erro ao receber dados da resposta");
    }
    
    return RMIMessage::deserialize(replyData);
}