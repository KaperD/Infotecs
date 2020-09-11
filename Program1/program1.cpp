#include <iostream>
#include <algorithm>

#include <thread>
#include <mutex>
#include <condition_variable>

#include "local_socket.h"

namespace {
// Позволяет потокам одновременно писать и читать строку
class SharedString {
public:
    void put(std::string s) {
        std::unique_lock<std::mutex> lock(mu);
        while (!data.empty()) { // while нужен, чтобы избежать ложного пробуждения (которое может произойти не по вине программы)
            cond.wait(lock);
        }
        data = std::move(s);
        cond.notify_all();
    }

    std::string get() {
        std::unique_lock<std::mutex> lock(mu);
        while (data.empty()) {
            cond.wait(lock);
        }
        std::string ret = std::move(data);
        data.clear();
        cond.notify_all();
        return ret;
    }

private:
    std::string data {};
    std::mutex mu {};
    std::condition_variable cond {};
};

void readStrings(SharedString& shareString) {
    std::string s;
    while (std::cin >> s) {
        if (s.length() <= 64 && std::all_of(s.begin(), s.end(), ::isdigit)) {
            std::sort(s.begin(), s.end(), std::greater());
            std::string final;
            int position = 1;
            for (char c : s) {
                if (position % 2 == 0) {
                    final.push_back('K');
                    final.push_back('B');
                } else {
                    final.push_back(c);
                }
                ++position;
            }
            shareString.put(final);
        }
    }
    shareString.put("quit");
}

void sendStrings(SharedString& share) {
    LocalSocketWrapper sock;
    sock.connectToServer();
    std::string s;
    while ((s = share.get()) != "quit") {
        std::cout << s << std::endl;
        int sum = 0;
        for (int k = 0; k < s.length(); k += 3) { // знает контракт, поэтому может обрабатывать так строку
            int x = static_cast<int>(s[k]) - static_cast<int>('0');
            sum += x;
        }
        sock.sendInt(sum);
    }
}

}
int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    SharedString ss;

    std::thread reader(readStrings, std::ref(ss));
    std::thread sender(sendStrings, std::ref(ss));

    sender.join();
    reader.join();


    return 0;
}



