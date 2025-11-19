// rmi_protocol.h
#ifndef RMI_PROTOCOL_H
#define RMI_PROTOCOL_H

#include <cstdint>
#include <vector>
#include <string>
#include <arpa/inet.h>

enum MessageType {
    REQUEST = 0,
    REPLY = 1
};

class RemoteObjectRef {
private:
    std::string objectId;
    std::string host;
    int port;
    
public:
    RemoteObjectRef(const std::string& objId = "", const std::string& h = "", int p = 0)
        : objectId(objId), host(h), port(p) {}
    
    // Serialização
    std::vector<char> serialize() const;
    static RemoteObjectRef deserialize(const std::vector<char>& data);
    
    // Getters
    std::string getObjectId() const { return objectId; }
    std::string getHost() const { return host; }
    int getPort() const { return port; }
};

class RMIMessage {
public:
    MessageType messageType;
    int requestId;
    RemoteObjectRef objectReference;
    std::string methodId;
    std::vector<char> arguments;
    
    RMIMessage() : messageType(REQUEST), requestId(0) {}
    
    // Serialização da mensagem completa
    std::vector<char> serialize() const;
    static RMIMessage deserialize(const std::vector<char>& data);
};

#endif