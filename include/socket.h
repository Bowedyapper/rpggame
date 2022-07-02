#pragma once
#include <sio_socket.h>
#include <sio_client.h>
#include <sio_message.h>

        
namespace Socket {
    sio::client client;
	std::string currentHost;
    int64_t latency = 0;
    bool connected = false;
    bool reconnecting = false;
    std::chrono::steady_clock::time_point lastLatencyCheck = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point lastLatencyPacketSent = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point lastLatencyPacketRecieved = std::chrono::steady_clock::now();
	/**
	 * @brief Event listener for events emitted from the server
	 * 
	 * @param eventName Case sensitive name of event
	 * @param func Pointer to a callback function or lambda
	 */
	void on(std::string eventName, const sio::socket::event_listener func);

	/**
	 * @brief Emits an event to the server with or without a list of inputs
	 * 
	 * @param name Case sensitive name of event
	 * @param msglist Optional list of input to be sent
	 * @param ack Optional pointer to ack callback function or lambda
	 */
	void emit(std::string const& name, sio::message::list const& msglist, std::function<void(sio::message::list const&)> const& ack);

	/**
	 * @brief Checks the latency between the client and server based on the time of an inital emit from the client and the time a response from the server is received
	 * 
	 * @param interval How long in milliseconds to wait to fire 
	 */
	void checkLatency(int interval);

    /**
	 * @brief Attempts to create a socket connection 
	 * 
	 * @param socketHost The host address to connect to
	 */
	void connect(const std::string socketHost);
}


void Socket::connect(const std::string socketHost) {
	currentHost = socketHost;
	client.connect(socketHost);
    on("latency_response", [&](sio::event& ev) {
			lastLatencyPacketRecieved = std::chrono::steady_clock::now();
			latency = std::chrono::duration_cast<std::chrono::milliseconds>(lastLatencyPacketRecieved - lastLatencyPacketSent).count();
		});

		client.set_open_listener([&](){
			utils::debugLog("info", "Connection to " + currentHost + " was successful");
			connected = true;
			reconnecting = false;
		});

		client.set_fail_listener([&](){
			utils::debugLog("error", "Failed to connect to " + currentHost);
		});

		client.set_reconnecting_listener([&](){
			utils::debugLog("info", "Reconnecting to " + currentHost);
			connected = false;
			reconnecting = true;
		});
}

void Socket::on(std::string eventName, const sio::socket::event_listener func) {
	client.socket()->on(eventName, func);
}

void Socket::emit(std::string const& name, sio::message::list const& msglist = nullptr, std::function<void(sio::message::list const&)> const& ack = nullptr)
{
	client.socket()->emit(name, msglist, ack);
}

void Socket::checkLatency(int interval) {
	int64_t elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - lastLatencyCheck).count();
	if (elapsed_time > interval) {
		emit("latency_check");
		lastLatencyPacketSent = std::chrono::steady_clock::now();
		lastLatencyCheck = std::chrono::steady_clock::now();
	}
}


