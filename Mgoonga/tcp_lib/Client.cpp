#include "pch.h"
#include "Client.h"
#include "Network.h"
#include "TCPConnection.h"
#include "Packet.h"

#include <iostream>

//---------------------------------------
bool Client::Connect(dbb::IPEndPoint ip)
{
    dbb::Socket m_socket;
   if (m_socket.Create(); m_socket.GetSocketError() == 0)
   {
       if (m_socket.SetBlocking(false) != dbb::PResult::SUCCESS)
           return false;
       std::cout << "dbb::Socket::Create is successful" << std::endl;
       if (m_socket.Connect(ip) == dbb::PResult::SUCCESS || m_socket.GetSocketError() == WSAEWOULDBLOCK)
       {
         std::cout << "socket.Connect is successful" << std::endl;
		     connection = { m_socket, ip };
		     master_fd.fd = m_socket.GetHandle();
		     master_fd.events = POLLRDNORM | POLLWRNORM;
		     master_fd.revents = 0;
         is_connected = true;
         ConnectionEstablished.Occur<const dbb::TCPConnection&>(connection);
         return true;
       }
       else
       {
           std::cout << "socket.Connect is UNsuccessful" << std::endl;
       }
   }
   else
   {
       std::cout << "socket.Create is fail" << std::endl;
   }
    return false;
}

//----------------------------------------
bool Client::IsConnected()
{
    return is_connected;
}

//--------------------------------------
bool Client::Frame()
{
	use_fd = master_fd;

	if (WSAPoll(&use_fd, 1, 1) > 0)
	{

		if (use_fd.revents & POLLERR) //If error occurred on this socket
		{
			//CloseConnection("POLLERR");
			return false;
		}

		if (use_fd.revents & POLLHUP) //If poll hangup occurred on this socket
		{
			//CloseConnection("POLLHUP");
			return false;
		}

		if (use_fd.revents & POLLNVAL) //If invalid socket
		{
			//CloseConnection("POLLNVAL");
			return false;
		}

		if (use_fd.revents & POLLRDNORM) //If normal data can be read without blocking
		{
           auto [bytesReceived, error] =  connection.Receive();
           if (bytesReceived <= 0)
           {
               CloseConnection(error);
               return false;
           }
           /*if (connection.IsCurrentPacketReceived())
               connection.AddSent("Hello col ne shutish!");*/
           return true;
		}

		if (use_fd.revents & POLLWRNORM) //If normal data can be written without blocking
		{
            connection.Send();
            if (connection.IsCurrentPacketSent()) //If full packet contents have been sent
               connection.ClearSentPacket();
		}
	}
	return true;
}

//------------------------------------------------------------------------
void Client::CloseConnection(const std::string& reason)
{
	std::cout << "[" << reason << "] Connection lost: " << connection.ToString() << "." << std::endl;
	connection.Close();
}

//------------------------------------------------------------------------
void Client::SendMsg(std::string&& msg)
{
    connection.AddSent(std::move(msg));
}

//------------------------------------------------------------------------
void Client::SendMsg(std::vector<uint32_t>&& msg)
{
  connection.AddSent(std::move(msg));
}

    //    dbb::Packet incomingPacket;
    //    if (connection.socket.Recv(incomingPacket) != dbb::PResult::SUCCESS)
    //    {
    //        return true;
    //        /*std::cout << "Lost connection?" << std::endl;
    //        is_connected = false;
    //        return false;*/
    //    }

    //    if (!ProcessPacket(incomingPacket))
    //    {
    //        is_connected = false;
    //        return false;
    //    }

    //return true;

   /* uint32_t a, b, c;
    a = 4;
    b = 6;
    c = 9;
    dbb::Packet stringPacket(dbb::PacketType::PT_ChatMessage);
    stringPacket << "This is my string! dbb";

    dbb::Packet intPacket(dbb::PacketType::PT_Intarray);
    intPacket << 3 << a << b << c;

    dbb::PResult result;
    if (rand() % 2)
        result = socket.Send(stringPacket);
    else
        result = socket.Send(intPacket);

    if (result != dbb::PResult::SUCCESS)
    {
        is_connected = false;
        return false;
    }
    std::cout << "attempting to send chunk of data" << std::endl;
    Sleep(500);
    return true;*/
