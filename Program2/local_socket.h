#ifndef SECOND_LOCAL_SOCKET_H
#define SECOND_LOCAL_SOCKET_H

// Нужны для работы с сокетами, чьтением и записью в них
#include <sys/socket.h>
#include <unistd.h>

#include <optional>
#include <exception>

class LocalSocketWrapper {
public:
    LocalSocketWrapper() {
        socketDescriptor = socket(PF_LOCAL, SOCK_STREAM, 0);
        if (socketDescriptor < 0) {
            throw std::runtime_error("Can't open socket");
        }
        params.sa_family = AF_LOCAL;
    }

    ~LocalSocketWrapper() {
        if (close(socketDescriptor) < 0) {
            std::cerr << "Error while closing socket\n";
        }
    }

    void becomeSerser() {
        if (bind(socketDescriptor, &params, sizeof(params)) < 0) {
            throw std::runtime_error("Can't bind socket");
        }
        if (listen(socketDescriptor, 1) < 0) {
            throw std::runtime_error("Can't listen socket");
        }

        int len = sizeof(params);
        if ((newSocketDescriptor = accept(socketDescriptor, &params, (socklen_t*)&len)) < 0) {
            throw std::runtime_error("Can't accept client");
        }
    }

    void connectToServer() {
        if (connect(socketDescriptor, &params, sizeof(params)) < 0) {
            throw std::runtime_error("Can't connect to server");
        }
        newSocketDescriptor = socketDescriptor;
    }

    void sendInt(int x) const {
        send(newSocketDescriptor, &x, sizeof(x), 0);
    }

    std::optional<int> getInt() const {
        int x {};
        int success = read(newSocketDescriptor, &x, sizeof(x));
        if (success < 0) {
            throw std::runtime_error("Can't read from socket");
        }
        if (success == 0) {
            return std::nullopt;
        }
        return x;
    }

private:
    int socketDescriptor {};
    sockaddr params {};
    int newSocketDescriptor {};
};

#endif //SECOND_LOCAL_SOCKET_H
