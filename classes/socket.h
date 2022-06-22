#include <sio_socket.h>
#include <sio_client.h>
#include <sio_message.h>

extern Game *game;
class Socket {

public:
	int lastLatencyCheck = clock();
	std::chrono::steady_clock::time_point lastLatencyPacketSent = std::chrono::steady_clock::now();
	std::chrono::steady_clock::time_point lastLatencyPacketRecieved = std::chrono::steady_clock::now();
	int latency = 0;
	Socket(char* socketHost) {
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
	bool connect(char* socketHost);
	
};

bool Socket::connect(char* socketHost) {
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

void Socket::checkLatency(int interval){
	if (clock() - lastLatencyCheck > interval) {
		emit("latency_check");
		lastLatencyPacketSent = std::chrono::steady_clock::now();
		lastLatencyCheck = clock();
	}
}
