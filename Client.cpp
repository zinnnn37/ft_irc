/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minjinki <minjinki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/22 09:25:20 by minjinki          #+#    #+#             */
/*   Updated: 2023/11/22 11:07:31 by minjinki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

int	Client::_clientNum = 1;

Client::Client() {}

Client::Client( const Clinet &c ) {}

Client::Client( int socket, std::string addr )
	: _isRegistered(false), _isClosed(false), _clientSoc(socket), _addr(addr)
{
	this->_nick = "client" + std::to_string(Client::_clientNum);
	Client::_clientNum++;
}

Client:~Client()
{
	close(this->_clientSoc);
}

Clinet	&Client::operator=( const Clinet &c )
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

void	Client::clearBuf()
{
	this->_buf.clear();
}

void	Client::appendBuf( const std::string &buf )
{
	this=->_buf.append(buf);
}
