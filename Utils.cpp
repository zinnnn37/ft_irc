/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minjinki <minjinki@student.42.kr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/19 10:57:22 by minjinki          #+#    #+#             */
/*   Updated: 2023/12/04 23:04:35 by minjinki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void	Server::_setNonBlock( int fd )
{
	int	flag;

	flag = fcntl(fd, F_SETFL, O_NONBLOCK);
	if (flag == ERROR)
		throw std::runtime_error("Exception: non-block error");
}

void	Server::_free()
{
	if (this->_serverSoc != FD_ERROR)
		close(this->_serverSoc);
	if (this->_kq != FD_ERROR)
		close(this->_kq);
	// for loop to delete all client socket & client
	ClientMap::iterator	it;
	for (it = this->_clients.begin(); it != this->_clients.end(); it++)
	{
		Client	*client = it->second;
		close(client->getSocket());
		delete client;
		client = 0;
	}

	// for loop to delete all channal
	ChannelMap::iterator	it2;
	for (it2 = this->_channels.begin(); it2 != this->_channels.end(); it2++)
	{
		delete it2->second;
		it2->second = 0;
	}
}

void	Server::_exit( const char *errmsg )
{
	this->_free();
	std::cerr << errmsg << std::endl;
	exit(FAILURE);
}
