/* Relógio Físico 
   Objetivo: Mostrar que a ordem dos eventos pode parecer incorreta sem controle lógico.*/

#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>
#include <algorithm>


std::mutex mtx;

long long tempoFisico() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
}

struct MensagemFisica {
    int origem;
    long long timestamp;
};

class ProcessoFisico {
public:
    int id;

    ProcessoFisico(int id) : id(id) {}

    void enviar(ProcessoFisico &destino) {
        std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 1000));

        long long t = tempoFisico();
        {
            std::lock_guard<std::mutex> lock(mtx);
            std::cout << "P" << id << " ENVIOU para P" << destino.id
                      << " | Tempo fisico = " << t << std::endl;
        }

        destino.receber({id, t});
    }

    void receber(MensagemFisica msg) {
        long long t = tempoFisico();
        std::lock_guard<std::mutex> lock(mtx);
        std::cout << "P" << id << " RECEBEU de P" << msg.origem
                  << " | Tempo local = " << t
                  << " | Tempo da msg = " << msg.timestamp << std::endl;
    }
};

int main() {
    srand(time(NULL));

    ProcessoFisico p0(0), p1(1), p2(2);

    std::thread t1(&ProcessoFisico::enviar, &p0, std::ref(p1));
    std::thread t2(&ProcessoFisico::enviar, &p1, std::ref(p2));
    std::thread t3(&ProcessoFisico::enviar, &p2, std::ref(p0));

    t1.join();
    t2.join();
    t3.join();

    return 0;
}
