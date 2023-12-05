/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minjinki <minjinki@student.42.kr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/19 10:57:22 by minjinki          #+#    #+#             */
/*   Updated: 2023/12/05 09:17:17 by minjinki         ###   ########.fr       */
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
}

void	Server::_exit( const char *errmsg )
{
	this->_free();
	std::cerr << errmsg << std::endl;
	exit(FAILURE);
}
