/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cmd_PART.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minjinki <minjinki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/02 18:35:31 by minjinki          #+#    #+#             */
/*   Updated: 2023/12/08 22:28:58 by minjinki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"

void Command::_clientLeaveChannel( Server *server, Client *client, Channel *channel )
{
	std::string ch_name = channel->getName();
	server->broadcast(ch_name, RPL_PART(client->getNick(), client->getNick(), ch_name));
	client->removeJoinedChannel(ch_name);
	channel->removeClient(client->getNick());
	if (channel->getClients().empty())
	{
		server->getChannels().erase(ch_name);
		delete channel;
		channel = 0;
	}
}

void Command::part( Server *server, Client *client, std::istringstream &buffer_stream )
{
	std::string response;
	std::string channel_line;

	buffer_stream >> channel_line;

	std::vector<std::string> v_channels;

	std::istringstream new_stream(channel_line);
	std::string channel;
	while (std::getline(new_stream, channel, ','))
	{
		this->_removeCRLF(channel);
		// std::cout << "channel : " << channel << std::endl;
		v_channels.push_back(channel);
	}

	for (std::vector<std::string>::iterator it = v_channels.begin(); it != v_channels.end(); it++)
	{
		if (server->getChannels().find(*it) == server->getChannels().end())
		{
			// 없는 채널일 경우 403
			client->setSendData(ERR_NOSUCHCHANNEL2(client->getNick(), *it) + CRLF);
			return ;
		}
		Channel *ch_po = server->getChannels()[*it];

		Client	*cl = ch_po->getClient(client->getNick());

		if (!cl)
		{
			// 채널에 유저가 없을 때 나가려고 하면 442 error
			client->setSendData(ERR_NOTONCHANNEL2(client->getNick(), *it) + CRLF);
			return ;
		}

		// std::set<Client *> users = ch_po->getClients();
		// for (ClientSet::iterator itss = users.begin(); itss != users.end(); ++itss) {
		// 	// 채널에 유저가 없을 때 나가려고 하면 442 error
		// 	client->setSendData(ERR_NOTONCHANNEL2(client->getNick(), *it) + CRLF);
		// 	return ;
		// }
		this->_clientLeaveChannel(server, client, ch_po);
		client->appendSendData(ERR_NOTONCHANNEL2(client->getNick(), *it) + CRLF);
	}
}
