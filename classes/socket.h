#include <sio_socket.h>
#include <sio_client.h>
#include <sio_message.h>

extern Game* game;
class Socket {

public:
	std::chrono::steady_clock::time_point lastLatencyCheck = std::chrono::steady_clock::now();
	std::chrono::steady_clock::time_point lastLatencyPacketSent = std::chrono::steady_clock::now();
	std::chrono::steady_clock::time_point lastLatencyPacketRecieved = std::chrono::steady_clock::now();
	int64_t latency = 0;
	Socket(const char* socketHost) {
		lastLatencyCheck = std::chrono::steady_clock::now();
		client.connect(socketHost);
		on("latency_response", [&](sio::event& ev) {
			lastLatencyPacketRecieved = std::chrono::steady_clock::now();
			latency = std::chrono::duration_cast<std::chrono::milliseconds>(lastLatencyPacketRecieved - lastLatencyPacketSent).count();
			});

	}
	void on(std::string eventName, const sio::socket::event_listener func);
	void emit(std::string const& name, sio::message::list const& msglist, std::function<void(sio::message::list const&)> const& ack);
	void checkLatency(int interval);
private:
	sio::client client;
	bool connect(const char* socketHost);

};

bool Socket::connect(const char* socketHost) {
	client.connect(socketHost);
	return true;
}

void Socket::on(std::string eventName, const sio::socket::event_listener func) {
	client.socket()->on(eventName, func);
}

void Socket::emit(std::string const& name, sio::message::list const& msglist = NULL, std::function<void(sio::message::list const&)> const& ack = NULL)
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
