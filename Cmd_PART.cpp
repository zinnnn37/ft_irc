/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cmd_PART.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minjinki <minjinki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/29 15:46:06 by minjinki          #+#    #+#             */
/*   Updated: 2023/11/29 17:03:48 by minjinki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"

void	Command::part( Server *server, Client *client, std::istringstream &iss )
{
	(void)server;
	(void)client;

	std::string	channelName;
	Channel	*ch;

	while (iss >> channelName)
	{
		if (channelName[channelName.size() - 1] == ',')
			channelName = channelName.substr(0, channelName.size() - 1);

		// channel이 존재하지 않는 경우
		ch = server->getChannel(channelName);
		if (!ch)
		{
			client->setSendData(ERR_NOSUCHCHANNEL(channelName) + CRLF);
			continue ;
		}

		// channel에 client가 없는 경우
		ch = client->findJoinedChannel(channelName);
		if (!ch)
		{
			client->setSendData(ERR_NOTONCHANNEL(channelName) + CRLF);
			continue ;
		}

		// channel에서 client를 제거
		ch->removeClient(client->getNick());
		ch->removeAuth(client->getNick());

		if (ch->getClients().size() == 0)
		{
			server->getChannels().erase(channelName);
			delete ch;
			ch = NULL;
		}

		// client에서 channel을 제거
		client->removeJoinedChannel(channelName);

		// broadcast
		server->broadcast(channelName, RPL_QUIT(client->getNick(), channelName));
	}
}
