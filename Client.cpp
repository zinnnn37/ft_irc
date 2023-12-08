/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minjinki <minjinki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/22 09:25:20 by minjinki          #+#    #+#             */
/*   Updated: 2023/12/09 01:40:56 by minjinki         ###   ########.fr       */
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

std::string	Client::getRealName() const
{
	return (this->_realName);
}

std::string	Client::getHostName() const
{
	return (this->_hostName);
}

std::string	Client::getServerName() const
{
	return (this->_serverName);
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

void	Client::setRealName( const std::string &realName )
{
	this->_realName = realName;
}

void	Client::setHostName( const std::string &hostName )
{
	this->_hostName = hostName;
}

void	Client::setServerName( const std::string &serverName )
{
	this->_serverName = serverName;
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

void	Client::appendSendData( std::string sendData )
{
	this->_sendData.append(sendData);
}

std::map<std::string, Channel *> Client::getChannels()
{
	return this->_joinedChannels;
}

std::string Client::getPrefix()
{
    std::string username = "!" + this->_userName;
    std::string hostname = "@" + this->_hostName;

    return this->_nick + username + hostname;
}

void Client::joinChannel( Channel *channel )
{
	this->_joinedChannels[channel->getName()] = channel;
}

void	Client::disconnectClientFromChannel(Server *server)
{
	for (ChannelMap::iterator it = this->_joinedChannels.begin();
	 		it != this->_joinedChannels.end(); it++)
	{
	 	(it)->second->removeClientinServer(server, *this);
	}

	this->_joinedChannels.clear();
	this->_invited.clear();
	this->_buf.clear();
	this->_sendData.clear();
}

ChannelMap	&Client::getJoinedChannel()
{
	return (this->_joinedChannels);
}

Channel	*Client::findJoinedChannel( std::string &channelName )
{
	return (this->_joinedChannels[channelName]);
}

ChannelSet	&Client::getInvited()
{
	return (this->_invited);
}

void	Client::addInvited( Channel *channel )
{
	this->_invited.insert(channel);
}

void	Client::removeJoinedChannel( std::string &channelName )
{
	this->_joinedChannels.erase(channelName);
}