/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minjinki <minjinki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/19 10:57:22 by minjinki          #+#    #+#             */
/*   Updated: 2023/11/19 11:00:33 by minjinki         ###   ########.fr       */
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
	// for loop to delete all client socket
	// for loop to delete all channal
}

void	Server::_exit( const char *errmsg )
{
	this->_free();
	std::cerr << errmsg << std::endl;
	exit(FAILURE);
}
