#include <iostream>
#include "local_socket.h"

int main() {
    LocalSocketWrapper sock;
    sock.becomeSerser();
    while (true) {
        auto opt = sock.getInt();
        if (!opt.has_value()) break;
        int x = opt.value();
        if (x > 99 && x % 32 == 0) {
            std::cout << "Данные получены: " << x << std::endl;
        } else {
            std::cout << "Ошибка, получено: " << x << std::endl;
        }
    }

    return 0;
}
