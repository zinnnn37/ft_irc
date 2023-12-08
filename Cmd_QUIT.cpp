/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cmd_QUIT.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minjinki <minjinki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/09 01:16:47 by minjinki          #+#    #+#             */
/*   Updated: 2023/12/09 01:26:39 by minjinki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"

void	Command::quit( Server *server, Client *client, std::istringstream &iss )
{
	std::string		message;

	std::getline(iss, message);

	if (message.empty())
		message = ":leaving";

	_removeCRLF(message);

	client->setSendData(ERR_QUIT(client->getPrefix(), message) + CRLF);

	std::string	res;

	res = RPL_QUIT(client->getPrefix(), message) + CRLF;

	//braodcast to all Channels
	ChannelMap channels = client->getChannels();

	for (ChannelMap::iterator it = channels.begin(); it != channels.end(); it++)
	{
		std::string	channelName = it->second->getName();
		server->broadcast(channelName, res);
	}

	client->setClosed(true);
}