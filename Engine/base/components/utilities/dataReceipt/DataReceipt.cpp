#include "DataReceipt.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>

#pragma comment(lib, "ws2_32.lib")

// std::mutex を使って標準出力を同期
std::mutex cout_mutex;

DataReceipt::DataReceipt() : port_(0), sock_(INVALID_SOCKET), is_running_(false) {
}

DataReceipt::~DataReceipt() {
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
	receiver_thread_ = std::thread(&DataReceipt::receiveLoop, this);
}

void DataReceipt::stop() {
	if (!is_running_) {
		return;
	}
	is_running_ = false;
	if (receiver_thread_.joinable()) {
		receiver_thread_.join();
	}
	closesocket(sock_);
}

bool DataReceipt::getReceivedMessage(std::string& message) {
	std::unique_lock<std::mutex> lock(queue_mutex_);
	queue_cv_.wait(lock, [this] { return !message_queue_.empty(); });

	if (!message_queue_.empty()) {
		message = message_queue_.front();
		message_queue_.pop();
		return true;
	}
	return false;
}

void DataReceipt::initializeWinsock() {
	if (WSAStartup(MAKEWORD(2, 2), &wsaData_) != 0) {
		throw std::runtime_error("WSAStartup failed");
	}
}

void DataReceipt::cleanupWinsock() {
	WSACleanup();
}

void DataReceipt::createSocket() {
	sock_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
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

void DataReceipt::receiveLoop() {
	char buffer[1024];
	sockaddr_in senderAddr;
	int senderAddrSize = sizeof(senderAddr);

	while (is_running_) {
		int recvLen = recvfrom(sock_, buffer, sizeof(buffer) - 1, 0, (struct sockaddr*)&senderAddr, &senderAddrSize);
		if (recvLen == SOCKET_ERROR) {
			std::cerr << "Recvfrom failed!" << std::endl;
			continue;
		}

		buffer[recvLen] = '\0';
		std::string message(buffer);

		{
			std::lock_guard<std::mutex> lock(queue_mutex_);
			message_queue_.push(message);
		}

		queue_cv_.notify_one(); // メッセージが届いたことを通知
	}
}

void DataReceipt::run() {
	try {
		start();

		// メインスレッドで受信したメッセージを表示
		while (is_running_) {
			std::string message;
			if (getReceivedMessage(message)) {
				// 出力を同期する
				std::lock_guard<std::mutex> lock_cout(cout_mutex);
				std::cout << "Received message: " << message << std::endl;
			}

			// ここで他の処理を加えたり、メッセージを処理したりできます
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}

		// 受信を停止
		stop();
	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		stop();
	}
}
