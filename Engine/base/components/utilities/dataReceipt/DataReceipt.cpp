#include "dataReceipt.h"

#include <chrono>
#include <thread>
#include <mutex>

#pragma comment(lib, "wsock32.lib") //winsock2.libは使えないのでwinsock32.lib をリンク

//std::mutexを使って標準出力を同期
std::mutex cout_mutex;

DataReceipt::DataReceipt() : port_(0), sock_(INVALID_SOCKET), is_running_(false) {
}

DataReceipt::~DataReceipt() {
    receipt3DList_.clear();
    stop();
    cleanupWinsock();
}

void DataReceipt::Initialize(int port) {
    port_ = port;
    initializeWinsock();
    createSocket();
    bindSocket();
}

void DataReceipt::start() {
    if (is_running_) {
        std::cerr << "Already receiving messages." << std::endl;
        return;
    }
    is_running_ = true;
}

void DataReceipt::stop() {
    if (!is_running_) {
        return;
    }
    is_running_ = false;
    closesocket(sock_);
}

bool DataReceipt::getReceivedMessage(std::string& message) {
    std::unique_lock<std::mutex> lock(queue_mutex_);

    //メッセージが届いていない場合でもブロックしない
    if (message_queue_.empty()) {
        return false;
    }

    message = message_queue_.front();
    message_queue_.pop();
    return true;
}

void DataReceipt::initializeWinsock() {
    //Winsock初期化
    if (WSAStartup(MAKEWORD(1, 1), &wsaData_) != 0) {
        throw std::runtime_error("WSAStartup failed");
    }
}

void DataReceipt::cleanupWinsock() {
    WSACleanup();//Winsock終了処理
}

void DataReceipt::createSocket() {
    sock_ = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_ == INVALID_SOCKET) {
        throw std::runtime_error("Socket creation failed");
    }
}

void DataReceipt::bindSocket() {
    recvAddr_.sin_family = AF_INET;
    recvAddr_.sin_addr.s_addr = INADDR_ANY;
    recvAddr_.sin_port = htons((u_short)port_);

    if (bind(sock_, (struct sockaddr*)&recvAddr_, sizeof(recvAddr_)) == SOCKET_ERROR) {
        throw std::runtime_error("Bind failed");
    }
}

//メッセージ受信を呼び出されるたびに確認
void DataReceipt::receiveMessage() {
    //タイムアウト設定
    setsockopt(sock_, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeoutMaxTime_, sizeof(timeoutMaxTime_));

    char buffer[1024];
    sockaddr_in senderAddr;
    int senderAddrSize = sizeof(senderAddr);

    int recvLen = recvfrom(sock_, buffer, sizeof(buffer) - 1, 0, (struct sockaddr*)&senderAddr, &senderAddrSize);
    if (recvLen == SOCKET_ERROR) {
        int error = WSAGetLastError();
        if (error == WSAETIMEDOUT) {
            // タイムアウトエラーが発生した場合、フリーズせずに戻る
            timeoutCount_++;
            if (timeoutCount_ >= timeoutMaxCount_) {
                isSceneDataSendEnd_ = true;
                timeoutCount_ = 0;
            }
            return;
        }
        std::cerr << "Recvfrom failed with error code: " << error << std::endl;
        return;
    }

    buffer[recvLen] = '\0';
    std::string message(buffer);

    {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        message_queue_.push(message);
    }

    queue_cv_.notify_one();//メッセージが届いたことを通知

    if (getReceivedMessage(message)) {
        //メッセージが届いていれば処理する
        Log(message);

        if (isSceneDataSendEnd_ == true) {
            receipt3DList_.clear();
            isSceneDataSendEnd_ = false;
        }

        auto receipt3D = std::make_unique<Receipt3D>();
        receipt3D->LoadFromString(message);
        receipt3D->Initialize();
        receipt3D->SetDirectionalLightFlag(true, 3);
        receipt3DList_.push_back(std::move(receipt3D));
    }
}

void DataReceipt::Draw(const ViewProjection& viewProjection) {
    for (const auto& receipt3D : receipt3DList_) {
        if (receipt3D) {
            receipt3D->Draw(viewProjection, { 1.0f,1.0f,1.0f,1.0f });
        }
    }
}