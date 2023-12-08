/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cmd_PING.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minjinki <minjinki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/09 01:10:49 by minjinki          #+#    #+#             */
/*   Updated: 2023/12/09 01:13:51 by minjinki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"

void	Command::ping( Server *server, Client *client, std::istringstream &iss )
{
	std::string		serverName;

	iss >> serverName;

	if (serverName.empty())
	{
		client->setSendData(ERR_NOORIGIN(client->getNick()) + CRLF);
		return ;
	}

	client->setSendData(RPL_PONG(client->getNick(), serverName) + CRLF);
}