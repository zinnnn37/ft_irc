/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cmd_TOPIC.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minjinki <minjinki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/02 15:32:26 by minjinki          #+#    #+#             */
/*   Updated: 2023/12/02 17:06:28 by minjinki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"

void	Command::topic( Server *server, Client *client, std::istringstream &iss )
{
	std::string		channelName;
	std::string		topic;

	iss >> channelName;
	iss >> topic;

	Channel *channel = server->getChannel(channelName);
	if (!channel)
	{
		client->setSendData(ERR_NOSUCHCHANNEL(channelName) + CRLF);
		return ;
	}

	if (!channel->isClient(client->getNick()))
	{
		client->setSendData(ERR_NOTONCHANNEL(channelName) + CRLF);
		return ;
	}

	if (!channel->isOperator(*client))
	{
		client->setSendData(ERR_CHANOPRIVSNEEDED(channelName) + CRLF);
		return ;
	}
}