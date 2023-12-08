/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cmd_KICK.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minjinki <minjinki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/02 13:33:51 by minjinki          #+#    #+#             */
/*   Updated: 2023/12/09 01:56:43 by minjinki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"

void	Command::kick( Server *server, Client *client, std::istringstream &iss )
{
	std::string		channelName;
	std::string		targets;
	std::string		message;

	iss >> channelName;
	iss >> targets;
	iss >> message;

	std::cout << "channelName: " << channelName << std::endl;
	std::cout << "targets: " << targets << std::endl;
	std::cout << "message: " << message << std::endl;
	

	if (channelName.empty() || targets.empty())
	{
		client->setSendData(ERR_NEEDMOREPARAMS(client->getNick(), std::string("KICK")) + CRLF + CRLF);
		return ;
	}

	// 채널이 존재하는지 확인
	Channel	*channel = server->getChannel(channelName);
	if (!channel)
	{
		client->setSendData(ERR_NOSUCHCHANNEL(client->getNick(), channelName) + CRLF + CRLF);
		return ;
	}

	std::cout << "channelName: " << channelName << std::endl;

	// client가 채널에 참가중인지 확인
	if (!channel->isClient(client->getNick()))
	{
		client->setSendData(ERR_NOTONCHANNEL(client->getNick(), channelName) + CRLF + CRLF);
		return ;
	}

	// client가 채널의 operator인지 확인
	if (!channel->isOperator(*client))
	{
		client->setSendData(ERR_CHANOPRIVSNEEDED(channelName) + CRLF + CRLF);
		return ;
	}

	while (!targets.empty())
	{
		std::string		target;
		size_t			pos = targets.find(',');

		if (pos == std::string::npos)
		{
			target = targets;
			targets.clear();
		}
		else
		{
			target = targets.substr(0, pos);
			targets = targets.substr(pos + 1);
		}

		std::cout << "KICK target: " << target << std::endl;

		// target이 채널에 참가중인지 확인
		if (!channel->isClient(target))
		{
			client->appendSendData(ERR_USERNOTINCHANNEL(client->getNick(), target, channelName) + CRLF + CRLF);
			continue ;
		}

		// target을 채널에서 제거
		Client	*targetClient = server->getClient(target);
		channel->removeClient(targetClient->getNick());
		channel->removeAuth(targetClient->getNick());
		targetClient->removeJoinedChannel(channelName);
		target += ":Welcome";
		server->broadcast(channelName, RPL_KICK(client->getNick(), channelName, target, message) + CRLF + CRLF);
		targetClient->setSendData(RPL_KICK(client->getNick(), channelName, target, message) + CRLF + CRLF);
	}
}