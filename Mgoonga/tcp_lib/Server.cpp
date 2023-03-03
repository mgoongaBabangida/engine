#include "pch.h"
#include "Server.h"
#include "Network.h"
#include "TCPConnection.h"
#include "Packet.h"

#include <iostream>

//---------------------------------------------------------------------
bool Server::Initialize(dbb::IPEndPoint ip)
{
    master_fd.clear();
    connections.clear();

    std::cout << "dbb::NetWork::Initialize is successful" << std::endl;
    if (listening_socket.Create(); listening_socket.GetSocketError() == 0)
    {
        std::cout << "dbb::listening_socket::Create is successful" << std::endl;
        if (listening_socket.Listen(ip) == dbb::PResult::SUCCESS)
        {
            std::cout << "listening_socket.Listen(test) is successful" << std::endl;

            WSAPOLLFD listeningSocketFD = {};
            listeningSocketFD.fd = listening_socket.GetHandle();
            listeningSocketFD.events = POLLRDNORM;
            listeningSocketFD.revents = 0;

            master_fd.push_back(listeningSocketFD);
        }
        return true;
    }
    else
    {
        std::cout << "listening_socket.Create is fail" << std::endl;
    }
    listening_socket.Close();
    return false;
}

//------------------------------------------------------------------------
bool Server::Frame()
{
    use_fd = master_fd;

    if (WSAPoll(use_fd.data(), use_fd.size(), 1) > 0)
    {
        WSAPOLLFD& listeningSocketFD = use_fd[0];
        if (listeningSocketFD.revents & POLLRDNORM)
        {
            if (auto accepted_socket = listening_socket.Accept(); accepted_socket.has_value())
            {
                dbb::TCPConnection connection(accepted_socket.value(), listening_socket.GetAcceptedEndPoint());
                std::cout << listening_socket.GetAcceptedEndPoint().m_ip_string << " listening_socket.Accept is successful" << std::endl;
                WSAPOLLFD newConnectionFD = {};
                newConnectionFD.fd = accepted_socket->GetHandle();
                newConnectionFD.events = POLLRDNORM | POLLWRNORM;
                newConnectionFD.revents = 0;
                master_fd.push_back(newConnectionFD);

                connection.AddSent("Welcome!");
                connections.push_back(connection);
            }
            else
            {
                std::cout << "socket.Accept is UNsuccessful" << std::endl;
            }
        }

        for (int i = use_fd.size() - 1; i >= 1; i--)
        {
            int connectionIndex = i - 1;
            dbb::TCPConnection& connection = connections[connectionIndex];

            if (use_fd[i].revents & POLLERR) //If error occurred on this socket
            {
                CloseConnection(connectionIndex, "POLLERR");
                continue;
            }

            if (use_fd[i].revents & POLLHUP) //If poll hangup occurred on this socket
            {
                CloseConnection(connectionIndex, "POLLHUP");
                continue;
            }

            if (use_fd[i].revents & POLLNVAL) //If invalid socket
            {
                CloseConnection(connectionIndex, "POLLNVAL");
                continue;
            }

            if (use_fd[i].revents & POLLRDNORM) //If normal data can be read without blocking
            {
                auto [bytesReceived, error] = connection.Receive();
                if (bytesReceived <= 0)
                {
                    CloseConnection(use_fd[i].fd, error);
                    continue;
                }
            }
            if (use_fd[i].revents & POLLWRNORM)
            {
                connection.Send();
                if (connection.IsCurrentPacketSent()) //If full packet contents have been sent
                    connection.ClearSentPacket();
            }
        }
    }
    return true;
}

//------------------------------------------------------------------------
void Server::CloseConnection(int connectionIndex, std::string reason)
{
	dbb::TCPConnection& connection = connections[connectionIndex];
	std::cout << "[" << reason << "] Connection lost: " << connection.ToString() << "." << std::endl;
	master_fd.erase(master_fd.begin() + (connectionIndex + 1));
	use_fd.erase(use_fd.begin() + (connectionIndex + 1));
	connection.Close();
	connections.erase(connections.begin() + connectionIndex);
}

//------------------------------------------
void Server::SendMsgToAll(std::string&& msg)
{
    for (int i = use_fd.size() - 1; i >= 1; i--)
    {
        int connectionIndex = i - 1;
        dbb::TCPConnection& connection = connections[connectionIndex];
        connection.AddSent(std::move(msg));
    }
}

