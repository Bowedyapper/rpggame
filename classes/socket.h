#include <sio_socket.h>
#include <sio_client.h>
#include <sio_message.h>

class Socket {

public:
	Socket(char* socketHost) {
		client.connect(socketHost);
	}
	void on(std::string eventName, const sio::socket::event_listener func);
	void emit(std::string const& name, sio::message::list const& msglist, std::function<void(sio::message::list const&)> const& ack);
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