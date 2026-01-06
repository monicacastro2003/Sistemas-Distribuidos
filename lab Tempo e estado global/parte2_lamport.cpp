/* Relógio de Lamport 
   Objetivo: Observar a consistência causal entre eventos.*/

#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>
#include <algorithm>

std::mutex mtx;

struct MensagemLamport {
    int origem;
    int timestamp;
};

class ProcessoLamport {
public:
    int id;
    int L = 0;

    ProcessoLamport(int id) : id(id) {}

    void eventoLocal(const std::string &desc) {
        std::lock_guard<std::mutex> lock(mtx);
        L++;
        std::cout << "P" << id << " EVENTO: " << desc
                  << " | L = " << L << std::endl;
    }

    void enviar(ProcessoLamport &destino) {
        std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 1000));

        eventoLocal("Enviando mensagem");
        destino.receber({id, L});
    }

    void receber(MensagemLamport msg) {
        std::lock_guard<std::mutex> lock(mtx);
        L = std::max(L, msg.timestamp) + 1;
        std::cout << "P" << id << " RECEBEU de P" << msg.origem
                  << " | L = " << L << std::endl;
    }
};

int main() {
    srand(time(NULL));

    ProcessoLamport p0(0), p1(1), p2(2);

    std::thread t1(&ProcessoLamport::enviar, &p0, std::ref(p1));
    std::thread t2(&ProcessoLamport::enviar, &p1, std::ref(p2));
    std::thread t3(&ProcessoLamport::enviar, &p2, std::ref(p0));

    t1.join();
    t2.join();
    t3.join();

    return 0;
}

