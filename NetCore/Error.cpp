#include "Error.h"

#include <unordered_map>
#include <Winsock2.h>

static std::unordered_map<int32, std::string> g_errorCodeStringMap;

std::string getWindowsErrorString(uint32 error) {
	LPVOID message = nullptr;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		error,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&message,
		0, NULL);
	std::string errorString((char*)message);

	LocalFree(message);

	return errorString;
}

std::string getWSAErrorString(int32 error) {
	auto iter = g_errorCodeStringMap.find(error);
	if (iter != g_errorCodeStringMap.end()) {
		return (*iter).second;
	}
	else {
		return "Unknown error.";
	}
}

void initErrorCodeStringMap() {
	g_errorCodeStringMap[WSANOTINITIALISED] = "WSA not initialized.";
	g_errorCodeStringMap[WSAENETDOWN] = "Network service provider failed.";
	g_errorCodeStringMap[WSAEAFNOSUPPORT] = "Unsupported address family.";
	g_errorCodeStringMap[WSAEINPROGRESS] = "WSA call already in progress.";
	g_errorCodeStringMap[WSAEMFILE] = "No more socket descriptors.";
	g_errorCodeStringMap[WSAEINVAL] = "Invalid argument.";
	g_errorCodeStringMap[WSAEINVALIDPROVIDER] = "Invalid service provider version.";
	g_errorCodeStringMap[WSAEINVALIDPROCTABLE] = "Service provider =ed invalid procedure table.";
	g_errorCodeStringMap[WSAENOBUFS] = "No more buffer space.";
	g_errorCodeStringMap[WSAEPROTONOSUPPORT] = "Unsupported protocol.";
	g_errorCodeStringMap[WSAEPROTOTYPE] = "Wrong protocol type for socket.";
	g_errorCodeStringMap[WSAEPROVIDERFAILEDINIT] = "Service provider not initialized.";
	g_errorCodeStringMap[WSAESOCKTNOSUPPORT] = "Unsupported socket type.";
	g_errorCodeStringMap[WSAEFAULT] = "Receive buffer or from buffer not in user address space.";
	g_errorCodeStringMap[WSAEINTR] = "Blocking canceled by WSACancelBlockingCall.";
	g_errorCodeStringMap[WSAEISCONN] = "Socket is connected, function not permitted for connected sockets.";
	g_errorCodeStringMap[WSAENETRESET] = "Time to live has expired for datagram packet.";
	g_errorCodeStringMap[WSAENOTSOCK] = "Socket descriptor is not a socket.";
	g_errorCodeStringMap[WSAEOPNOTSUPP] = "Operation not supported for this type of socket.";
	g_errorCodeStringMap[WSAESHUTDOWN] = "Socket has been shutdown.";
	g_errorCodeStringMap[WSAEWOULDBLOCK] = "Socket would have blocked.";
	g_errorCodeStringMap[WSAEMSGSIZE] = "Message was to big to fit in buffer and was truncated.";
	g_errorCodeStringMap[WSAETIMEDOUT] = "Socket timed out.";
	g_errorCodeStringMap[WSAECONNRESET] = "Virtual circuit was reset by remote side.";
	g_errorCodeStringMap[WSAENOTCONN] = "Socket is not connected.";
	g_errorCodeStringMap[WSAECONNABORTED] = "The virtual circuit was terminated due to a time-out or other failure.";
}