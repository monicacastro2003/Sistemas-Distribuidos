/* Relógio Vetorial
   Objetivo: Determinar a concorrência entre eventos usando vetores. */

#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>
#include <algorithm>

std::mutex mtx;

struct MensagemVetorial {
    int origem;
    std::vector<int> vetor;
};

class ProcessoVetorial {
public:
    int id;
    std::vector<int> V;

    ProcessoVetorial(int id, int n) : id(id), V(n, 0) {}

    void eventoLocal(const std::string &desc) {
        std::lock_guard<std::mutex> lock(mtx);
        V[id]++;
        std::cout << "P" << id << " EVENTO: " << desc
                  << " | V = ";
        imprimir();
    }

    void enviar(ProcessoVetorial &destino) {
        std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 1000));
        eventoLocal("Enviando mensagem");
        destino.receber({id, V});
    }

    void receber(MensagemVetorial msg) {
        std::lock_guard<std::mutex> lock(mtx);
        for (size_t i = 0; i < V.size(); i++) {
            V[i] = std::max(V[i], msg.vetor[i]);
        }
        V[id]++;
        std::cout << "P" << id << " RECEBEU de P" << msg.origem
                  << " | V = ";
        imprimir();
    }

    void imprimir() {
        std::cout << "[ ";
        for (int x : V) std::cout << x << " ";
        std::cout << "]" << std::endl;
    }
};

int main() {
    srand(time(NULL));

    int N = 3;
    ProcessoVetorial p0(0, N), p1(1, N), p2(2, N);

    std::thread t1(&ProcessoVetorial::enviar, &p0, std::ref(p1));
    std::thread t2(&ProcessoVetorial::enviar, &p1, std::ref(p2));
    std::thread t3(&ProcessoVetorial::enviar, &p2, std::ref(p0));

    t1.join();
    t2.join();
    t3.join();

    return 0;
}

