/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cmd_PART_ori.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minjinki <minjinki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/29 15:46:06 by minjinki          #+#    #+#             */
/*   Updated: 2023/12/02 18:43:31 by minjinki         ###   ########.fr       */
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
	std::string	tmp;
	std::string	channelName;
	Channel	*ch;

	iss >> channelName;
	std::istringstream	iss2(channelName);

	while (getline(iss2, tmp, ','))
	{
		this->_removeCRLF(channelName);

		std::cout << "channelName: " << channelName << std::endl;

		// channel이 존재하지 않는 경우
		ch = server->getChannel(channelName);
		if (!ch)
		{
			client->appendSendData(ERR_NOSUCHCHANNEL(channelName) + CRLF);
			continue ;
		}

		// channel에 client가 없는 경우
		ch = client->findJoinedChannel(channelName);
		if (!ch)
		{
			client->setSendData(ERR_NOTONCHANNEL(channelName) + CRLF);
			continue ;
		}

		server->broadcast(channelName, RPL_PART(client->getNick(), channelName) + CRLF);

		// client에서 channel을 제거
		client->removeJoinedChannel(channelName);
		
		// channel에서 client를 제거
		ch->removeClient(client->getNick());
		ch->removeAuth(client->getNick());

		if (ch->getClients().size() == 0)
		{
			server->getChannels().erase(channelName);
			// removeChannel(server->getChannels(), channelName);
			delete ch;
			ch = 0;
		}
	}
}