/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cmd_PART.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minjinki <minjinki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/02 18:35:31 by minjinki          #+#    #+#             */
/*   Updated: 2023/12/02 19:01:10 by minjinki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"

void Command::_clientLeaveChannel( Server *server, Client *client, Channel *channel )
{
	std::cout << "-------------" << std::endl;
	std::string ch_name = channel->getName();
	server->broadcast(ch_name, RPL_PART(client->getPrefix(), ch_name));
	client->appendSendData(RPL_PART(client->getPrefix(), ch_name) + CRLF);
	client->appendSendData(RPL_PART(client->getPrefix(), ch_name) + CRLF);
	std::string channel_nickname = channel->getName();
	client->removeJoinedChannel(channel_nickname);
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
		channel.erase(std::remove(channel.begin(), channel.end(), '\r'));
		channel.erase(std::remove(channel.begin(), channel.end(), '\n'));
		v_channels.push_back(channel);
	}

	for (std::vector<std::string>::iterator it = v_channels.begin(); it != v_channels.end(); it++)
	{
		std::cout << "=======================" << std::endl;
		std::cout << "=======================" << std::endl;
		std::cout << "=======================" << std::endl;
		std::cout << "=======================" << std::endl;
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
	}
	client->setSendData(response + CRLF);
}
