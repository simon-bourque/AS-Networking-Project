#pragma once

#include "ThreadPool.h"
#include "OverlappedBuffer.h"
#include "IPV4Address.h"

#include <string>
#include <Windows.h>

class TCPSocket;

class Connection {
public:
	enum class ConnectionState {
		DISCONNECTED,
		CONNECTED
	};
private:
	ConnectionState m_state;

	IPV4Address m_address;
	std::string m_uniqueName;

	TCPSocket* m_tcpSocket;
	OverlappedBuffer m_overlappedBuffer;

	uint32 m_offerReqNumber;
	uint32 m_lastItemOfferedID;
public:
	Connection();
	Connection(const std::string& name, const IPV4Address& address);
	virtual ~Connection();

	void connect(TCPSocket&& socket, HANDLE ioCompletionPort);
	void shutdown();

	OverlappedBuffer& getOverlappedBuffer() { return m_overlappedBuffer; }
	const IPV4Address& getAddress() const { return m_address; }

	bool isConnected() const { return m_state == ConnectionState::CONNECTED; }

	void setOfferReqNumber(uint32 offerRequestNumber) { m_offerReqNumber = offerRequestNumber; }
	uint32 getOfferReqNumber() const { return m_offerReqNumber; }

	void setLastItemOfferedID(uint32 itemOfferedID) { m_lastItemOfferedID = itemOfferedID; }
	uint32 getLastItemOfferedID() const { return m_lastItemOfferedID; }
};

