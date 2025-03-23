/**
 * @file DataReceipt.cpp
 * @brief Houdiniから受信したデータを解析、描画クラスへ送信するクラス
 * @author KATO
 * @date 2025/01/07
 */

#include "dataReceipt.h"

#include <chrono>
#include <thread>
#include <mutex>

#pragma comment(lib, "wsock32.lib")

std::mutex cout_mutex;

#define BUFFER_SIZE 65536 // 受信バッファのサイズ（64KB）
#define DECOMPRESSED_BUFFER_MULTIPLIER 10 // 解凍バッファの倍率

DataReceipt::DataReceipt() : port_(0), sock_(INVALID_SOCKET), is_running_(false) {}

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
    if (message_queue_.empty()) {
        return false;
    }
    message = message_queue_.front();
    message_queue_.pop();
    return true;
}

void DataReceipt::initializeWinsock() {
    if (WSAStartup(MAKEWORD(1, 1), &wsaData_) != 0) {
        throw std::runtime_error("WSAStartup failed");
    }
}

void DataReceipt::cleanupWinsock() {
    WSACleanup();
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

void DataReceipt::receiveMessage() {
    // 受信バッファサイズを拡張（デフォルトより大きくする）
    int recvBufSize = 1024 * 1024 * 10; // 10MB
    setsockopt(sock_, SOL_SOCKET, SO_RCVBUF, (char*)&recvBufSize, sizeof(recvBufSize));

    // タイムアウト設定
    setsockopt(sock_, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeoutMaxTime_, sizeof(timeoutMaxTime_));

    std::vector<char> receivedData;
    char buffer[BUFFER_SIZE];
    sockaddr_in senderAddr;
    int senderAddrSize = sizeof(senderAddr);

    while (true) {
        int recvLen = recvfrom(sock_, buffer, sizeof(buffer) - 1, 0, (struct sockaddr*)&senderAddr, &senderAddrSize);
        if (recvLen == SOCKET_ERROR) {
            int error = WSAGetLastError();
            if (error == WSAETIMEDOUT) {
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

        buffer[recvLen] = '\0';  // 受信データをnull終端文字列にする
        receivedData.insert(receivedData.end(), buffer, buffer + recvLen);  // データを蓄積

        // `/add` があるかチェック
        std::string receivedString(receivedData.begin(), receivedData.end());
        if (receivedString.size() >= 4 && receivedString.substr(receivedString.size() - 4) == "/add") {
            receivedData.resize(receivedData.size() - 4);  // `/add` を削除
            continue;  // 次のデータを待つ
        }

        break;  // `/add` が無い場合は終了
    }

    // Zlibを使用してデータを解凍
    uLongf decompressedLen = static_cast<uLongf>(receivedData.size() * DECOMPRESSED_BUFFER_MULTIPLIER);  // 解凍後の予想サイズ
    std::vector<char> decompressedData(decompressedLen);

    int ret = uncompress(reinterpret_cast<Bytef*>(decompressedData.data()), &decompressedLen,
        reinterpret_cast<const Bytef*>(receivedData.data()), static_cast<uLong>(receivedData.size()));

    if (ret == Z_BUF_ERROR) {
        // バッファが足りない場合、再度確保して解凍
        decompressedLen = static_cast<uLongf>(receivedData.size() * DECOMPRESSED_BUFFER_MULTIPLIER);
        decompressedData.resize(decompressedLen);
        ret = uncompress(reinterpret_cast<Bytef*>(decompressedData.data()), &decompressedLen,
            reinterpret_cast<const Bytef*>(receivedData.data()), static_cast<uLong>(receivedData.size()));
    }

    if (ret != Z_OK) {
        std::cerr << "Data decompression failed with error code: " << ret << std::endl;
        return;
    }

    std::string message(decompressedData.begin(), decompressedData.begin() + decompressedLen);  // 解凍データを文字列化

    {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        message_queue_.push(message);  // メッセージキューに追加
    }
    queue_cv_.notify_one();  // メッセージ受信を通知

    // 受信データの処理
    if (getReceivedMessage(message)) {
        Log(message);

        if (isSceneDataSendEnd_) {
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
            receipt3D->Draw(viewProjection, { 1.0f, 1.0f, 1.0f, 1.0f });
        }
    }
}