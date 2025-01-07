#pragma once
#include <string>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <winsock.h>
#include <iostream>

class DataReceipt {
public:
    DataReceipt();
    ~DataReceipt();

    void Initialize(int port);    // 初期化メソッド
    void start();
    void stop();
    bool getReceivedMessage(std::string& message);
    void receiveMessage();   // メッセージを確認する関数に変更

private:
    void initializeWinsock();
    void cleanupWinsock();
    void createSocket();
    void bindSocket();

    int port_;
    SOCKET sock_;
    struct sockaddr_in recvAddr_;
    WSADATA wsaData_;
    bool is_running_;
    std::queue<std::string> message_queue_;
    std::mutex queue_mutex_;
    std::condition_variable queue_cv_;
};