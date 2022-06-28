/*****************************************************************//**
 * @file   socket.h
 * @brief  Creates a socket instance with the socket server and allows
 *         for slightly easier usage instead of using the socket.io
 *         library as is, I wanted to add my own methods to the socket
 *         instance for convience.
 * 
 * @author Jason Hewitt <bowedyapper@live.co.uk>
 * @date   June 2022
 *********************************************************************/

#include <sio_socket.h>
#include <sio_client.h>
#include <sio_message.h>

extern Game* game;

class Socket {
public:
	int64_t latency = 0;
	std::chrono::steady_clock::time_point lastLatencyCheck = std::chrono::steady_clock::now();
	std::chrono::steady_clock::time_point lastLatencyPacketSent = std::chrono::steady_clock::now();
	std::chrono::steady_clock::time_point lastLatencyPacketRecieved = std::chrono::steady_clock::now();
	
	/**
	 * @brief Constructs a new Socket object, sets up a connection and starts listening for latency responses from server
	 * 
	 * @param socketHost The host address to connect to
	 */
	Socket(const char* socketHost) {
		lastLatencyCheck = std::chrono::steady_clock::now();
		client.connect(socketHost);
		on("latency_response", [&](sio::event& ev) {
			lastLatencyPacketRecieved = std::chrono::steady_clock::now();
			latency = std::chrono::duration_cast<std::chrono::milliseconds>(lastLatencyPacketRecieved - lastLatencyPacketSent).count();
		});
	}

	/**
	 * @brief Event listener for events emitted from the server
	 * 
	 * @param eventName Case sensitive name of event
	 * @param func Pointer to a callback function
	 */
	void on(std::string eventName, const sio::socket::event_listener func);

	/**
	 * @brief Emits an event to the server with or without a list of inputs
	 * 
	 * @param name Case sensitive name of event
	 * @param msglist Optional list of input to be sent
	 * @param ack Optional pointer to ack callback function
	 */
	void emit(std::string const& name, sio::message::list const& msglist, std::function<void(sio::message::list const&)> const& ack);

	/**
	 * @brief Checks the latency between the client and server based on the time of an inital emit from the client and the time a response from the server is received
	 * 
	 * @param interval How long in milliseconds to wait to fire 
	 */
	void checkLatency(int interval);

private:
	sio::client client;

	/**
	 * @brief Attempts to create a socket connection 
	 * 
	 * @param socketHost The host address to connect to
	 * @return true if socket connection was successful
	 * @return false if an error was thrown
	 */
	bool connect(const char* socketHost);

};

bool Socket::connect(const char* socketHost) {
	client.connect(socketHost);
	return true;
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
