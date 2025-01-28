#pragma once
#include <string>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <winsock.h>
#include <iostream>

#include "Receipt3D.h"

class DataReceipt {
public:
    DataReceipt();
    ~DataReceipt();

    void Initialize(int port);//初期化メソッド
    void start();
    void stop();
    bool getReceivedMessage(std::string& message);
    void receiveMessage();//メッセージを確認する関数に変更

    void Draw(const ViewProjection& viewProjection);

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

    bool isSceneDataSendEnd_ = false;
    int timeoutCount_ = 0;//現在の連続タイムアウト回数
    const int timeoutMaxCount_ = 30;//この値以上連続でタイムアウトしていた場合、新規データ受信時に新シーンデータと判定する
    const int timeoutMaxTime_ = 10;//ミリ秒単位でタイムアウト時間を設定

    std::list<std::unique_ptr<Receipt3D>> receipt3DList_;
};