/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cmd_PART.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minjinki <minjinki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/29 15:46:06 by minjinki          #+#    #+#             */
/*   Updated: 2023/11/29 17:09:13 by minjinki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"


// void Server::clientLeaveChannel(Client &client, Channel *channel)
// {
// 	std::string ch_name = channel->getName();
// 	broadcast(ch_name, RPL_PART(client.getPrefix(), ch_name));
// 	client.leaveChannel(channel);
// 	channel->deleteClient(client.getNickname());
// 	if (channel->getUsers().size() == 0)
// 	{
// 		this->channels.erase(ch_name);
// 		delete channel;
// 		channel = 0;
// 	}
// }


// /connect 10.16.1.8 1111 1234
void removeChannel(ChannelMap &channels, const std::string &channelName) {
    ChannelMap::iterator iterator = channels.find(channelName);
    if (iterator != channels.end()) {
        delete iterator->second; // 해당 채널 객체를 메모리에서 해제
        channels.erase(iterator); // map에서 제거
    } else {
        std::cout << "Channel not found: " << channelName << std::endl;
    }
}

void	Command::part( Server *server, Client *client, std::istringstream &iss )
{
	std::string	channelName;
	Channel	*ch;

	while (iss >> channelName)
	{
		// std::cout << "hello" << std::endl;
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

		server->broadcast(channelName, RPL_QUIT(client->getNick(), channelName));
		
		// channel에서 client를 제거
		ch->removeClient(client->getNick());
		ch->removeAuth(client->getNick());
		// client에서 channel을 제거
		client->removeJoinedChannel(channelName);

		if (ch->getClients().size() == 0)
		{
			server->getChannels().erase(channelName);
			// removeChannel(server->getChannels(), channelName);
			delete ch;
			ch = 0;
		}
	}
}
