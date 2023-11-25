/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minjinki <minjinki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/22 09:25:20 by minjinki          #+#    #+#             */
/*   Updated: 2023/11/25 10:53:26 by minjinki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

int	Client::_clientNum = 1;

Client::Client() {}

Client::Client( const Client &c )
{
	(void)c;
}

Client::Client( int socket, std::string addr )
	: _isRegistered(false), _isClosed(false), _clientSoc(socket), _addr(addr)
{
	this->_nick = "client" + std::to_string(Client::_clientNum);
	Client::_clientNum++;
}

Client::~Client()
{
	close(this->_clientSoc);
}

Client	&Client::operator=( const Client &c )
{
	(void)c;
	return (*this);
}

bool	Client::isRegistered() const
{
	return (this->_isRegistered);
}

bool	Client::isClosed() const
{
	return (this->_isClosed);
}

void	Client::setRegistered( bool isRegistered )
{
	this->_isRegistered = isRegistered;
}

void	Client::setClosed( bool isClosed )
{
	this->_isClosed = isClosed;
}

int	Client::getSocket() const
{
	return (this->_clientSoc);
}

std::string	Client::getAddr() const
{
	return (this->_addr);
}

std::string	Client::getNick() const
{
	return (this->_nick);
}

std::string	Client::getUserName() const
{
	return (this->_userName);
}

std::string	Client::getBuf() const
{
	return (this->_buf);
}

std::string	Client::getSendData() const
{
	return (this->_sendData);
}

void	Client::setNick( const std::string &nick )
{
	this->_nick = nick;
}

void	Client::setUserName( const std::string &userName )
{
	this->_userName = userName;
}

void	Client::setBuf( std::string &buf )
{
	this->_buf = buf;
}

void	Client::setBuf( std::string buf )
{
	this->_buf = buf;
}

void	Client::setSendData( std::string sendData )
{
	this->_sendData = sendData;
}

void	Client::clearBuf()
{
	this->_buf.clear();
}

void	Client::clearSendData()
{
	this->_sendData.clear();
}

void	Client::appendBuf( std::string buf )
{
	this->_buf.append(buf);
}

void	Client::disconnectClientFromChannel()
{
	// size_t	size;

	// for (ChannelVec::iterator it = this->_joinedChannels.begin();
	// 		it != this->_joinedChannels.end(); it++)
	// {
	// 	size = (*it)->getClients().size();
	// 	(*it)->removeClient(client);

	// 	// 클라이언트가 채널에 남아있는 유일한 클라이언트였다면 채널 삭제
	// 	if (size == 1)
	// 		this->_server->removeChannel(*it);
	// }
}
