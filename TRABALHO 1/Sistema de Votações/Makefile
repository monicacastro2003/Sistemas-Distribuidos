CXX = g++
CXXFLAGS = -std=c++17 -O2 -pthread
LDFLAGS = -lprotobuf -pthread

PROTOC = protoc
PROTO_SRC = voting.proto

all: genproto server voter_client admin_client multicast_listener

genproto:
	$(PROTOC) --cpp_out=. $(PROTO_SRC)

server: server.cpp voting.pb.cc common.h
	$(CXX) $(CXXFLAGS) server.cpp voting.pb.cc -o server $(LDFLAGS)

voter_client: voter_client.cpp voting.pb.cc common.h
	$(CXX) $(CXXFLAGS) voter_client.cpp voting.pb.cc -o voter_client $(LDFLAGS)

admin_client: admin_client.cpp voting.pb.cc
	$(CXX) $(CXXFLAGS) admin_client.cpp voting.pb.cc -o admin_client $(LDFLAGS)

multicast_listener: multicast_listener.cpp voting.pb.cc
	$(CXX) $(CXXFLAGS) multicast_listener.cpp voting.pb.cc -o multicast_listener $(LDFLAGS)

clean:
	rm -f server voter_client admin_client multicast_listener voting.pb.cc voting.pb.h *.o
