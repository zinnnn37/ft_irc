/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minjinki <minjinki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/25 10:51:07 by minjinki          #+#    #+#             */
/*   Updated: 2023/11/25 11:57:19 by minjinki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"
#include "Client.hpp"
#include "Define.hpp"

class	Server;

Command::Command() {}

Command::Command( const Command &c )
{
	(void)c;
}

Command::Command( Server *server )
	: _server(server)
{
}

Command::~Command() {}

Server	*Command::_getServer() const
{
	return (this->_server);
}
