#include "OSCInputDeviceSample_socket.h"

// oscpack
#include "osc/OscReceivedElements.h"
#include "osc/OscException.h"

#define INPUT_BUFFER_SIZE 1024

// Constructor
OSCInputDeviceSampleWinSocket::OSCInputDeviceSampleWinSocket()
{
    // Initialize Windows Sockets API
    int startup = WSAStartup(MAKEWORD(2, 2), &wsadata);

    if (startup != 0)
        mSocketStatus = "Error: WSAStartup failed with Windows Sockets Error Code: " + std::to_string(startup);
}

// Destructor
OSCInputDeviceSampleWinSocket::~OSCInputDeviceSampleWinSocket()
{
    WSACleanup(); // Release Windows Sockets API
}

void OSCInputDeviceSampleWinSocket::SetupDeviceConnection(OSCInputDeviceSampleDevice *pParent)
{
    mOSCInputDevice = pParent; // Get the handle to the device
}

bool OSCInputDeviceSampleWinSocket::CreateSampleSocket()
{
    SOCKET newsocket;
    newsocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (newsocket == INVALID_SOCKET)
        mSocketStatus = "Error: Failed to create socket, Windows Sockets Error Code: " + std::to_string(WSAGetLastError());
    else
    {
        serverSocket = newsocket;

        // Set the socket as non blocking
        u_long mode = 1;
        ioctlsocket(serverSocket, FIONBIO, &mode);

        mSocketStatus = "Socket created successfulky, serverSocket: " + std::to_string(serverSocket);
        return true;
    }
    return false;
}

// Close plugin socket
void OSCInputDeviceSampleWinSocket::CloseSampleSocket()
{
    closesocket(serverSocket);
    serverSocket = INVALID_SOCKET;
}

bool OSCInputDeviceSampleWinSocket::BindSocket(int port, unsigned int ipAddr)
{
    bool lSuccess = false;

    if (serverSocket != INVALID_SOCKET)
    {
        // Configure UDP port
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(port);
        serverAddr.sin_addr.s_addr = ipAddr;

        // Bind the socket to the address and port
        int result = bind(serverSocket, (sockaddr *)&serverAddr, sizeof(serverAddr));

        if (result == SOCKET_ERROR)
            mSocketStatus = "Error: Failed to bind socket, Windows Sockets Error Code: " + std::to_string(WSAGetLastError());
        else
        {
            // Get address information
            sockaddr_in addr;
            int addr_len = sizeof(addr);
            if (getsockname(serverSocket, (sockaddr *)&addr, &addr_len) == 0)
            {
                char ip[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &addr.sin_addr, ip, sizeof(ip));
                lSuccess = true;

                mSocketStatus = "Socket bound successfully to address: " + std::string(ip) + ", port: " + std::to_string(ntohs(addr.sin_port));
            }
            else
                mSocketStatus = "Error: Failed to retrieve bound address info, Windows Sockets Error Code: " + std::to_string(WSAGetLastError());
        }
    }
    return lSuccess;
}

// Receive data from socket and Store it in private member mData
int OSCInputDeviceSampleWinSocket::ReceiveData()
{
    int recvLen = 0;
    if (serverSocket == INVALID_SOCKET)
        return 0;

    char buffer[INPUT_BUFFER_SIZE];
    sockaddr_in clientAddr;
    int clientAddrSize = sizeof(clientAddr);

    recvLen = recvfrom(serverSocket, buffer, INPUT_BUFFER_SIZE, 0, (sockaddr *)&clientAddr, &clientAddrSize);

    if (recvLen == SOCKET_ERROR)
    {
        if (WSAGetLastError() == WSAEWOULDBLOCK)
            return 0; // No data received, non-blocking mode

        return SOCKET_ERROR; // Other error
    }
    else if (recvLen > 0)
    {
        try
        {
            osc::ReceivedPacket p(buffer, recvLen);

            // Process if message, discard if bundle
            if (p.IsMessage())
            {
                osc::ReceivedMessage m(p);

                // example：Head
                if (strcmp(m.AddressPattern(), "/VMC/Ext/Bone/Pos") == 0 && m.ArgumentCount() == 8)
                {
                    auto arg = m.ArgumentsBegin();
                    const char *boneName = (arg++)->AsString();

                    if (strcmp(boneName, "Head") == 0)
                    {
                        mData.position[0] = (arg++)->AsFloat();
                        mData.position[1] = (arg++)->AsFloat();
                        mData.position[2] = (arg++)->AsFloat();

                        mData.rotation[0] = (arg++)->AsFloat();
                        mData.rotation[1] = (arg++)->AsFloat();
                        mData.rotation[2] = (arg++)->AsFloat();
                        mData.rotation[3] = (arg++)->AsFloat();
                    }
                }
            }
        }
        catch (const osc::Exception &e)
        {
            mSocketStatus = "Error: Failed to process OSC message, exception: " + std::string(e.what());
            return SOCKET_ERROR;
        }
    }

    return recvLen;
}

// Called by device to pull data from socket
BoneTransform *OSCInputDeviceSampleWinSocket::PassData()
{
    return &mData;
}