#ifndef RMI_CORE_H
#define RMI_CORE_H

#include "rmi_protocol.h"
#include "remote_service.h"
#include <vector>
#include <string>
#include <map>
#include <memory>
#include <stdexcept>

// Declarações COMPLETAS das classes
class RMIServer {
private:
    int serverPort;
    int serverSocket;
    std::map<std::string, IRemoteSupermercadoService*> remoteObjects;
    int nextRequestId;
    
    // Métodos privados
    void handleClient(int clientSocket);
    std::vector<char> executeMethod(const RMIMessage& request);
    std::vector<char> serializeBool(bool value);
    std::vector<char> serializeInt(int value);
    std::vector<char> serializeDouble(double value);
    std::vector<char> serializeString(const std::string& str);
    int deserializeInt(const std::vector<char>& data);
    std::string deserializeString(const std::vector<char>& data);
    
public:
    RMIServer(int port);
    bool start();
    void registerObject(const std::string& objectId, IRemoteSupermercadoService* service);
    void run();
    RMIMessage getRequest(int clientSocket);
    void sendReply(const std::vector<char>& reply, int clientSocket);
};

class RMIClient {
private:
    std::string serverHost;
    int serverPort;
    RMIMessage receiveReply(int clientSocket);
    
public:
    RMIClient(const std::string& host, int port);
    std::vector<char> doOperation(const RemoteObjectRef& remoteObject, 
                                 const std::string& methodId, 
                                 const std::vector<char>& arguments);
};

#endif