/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cmd_INVITE.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minjinki <minjinki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/29 12:54:15 by minjinki          #+#    #+#             */
/*   Updated: 2023/12/09 02:28:36 by minjinki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"
#include "Channel.hpp"

void	Command::invite( Server *server, Client *client, std::istringstream &iss )
{
	std::string	target;
	std::string	channelName;

	iss >> target;
	iss >> channelName;

	// client가 channel에 없는 경우
	Channel	*ch = client->findJoinedChannel(channelName);
	if (!ch)
	{
		client->setSendData(ERR_NOTONCHANNEL(client->getNick(), channelName) + CRLF + CRLF);
		return ;
	}

	// target이 존재하지 않는 경우
	Client	*cl = server->getClient(target);
	if (!cl)
	{
		client->setSendData(ERR_NOSUCHNICK(client->getNick(), target) + CRLF + CRLF);
		return ;
	}
	
	// target이 이미 channel에 있는 경우
	if (ch->isClient(cl->getNick()))
	{
		client->setSendData(ERR_USERONCHANNEL(client->getNick(), channelName, target) + CRLF + CRLF);
		return ;
	}

	if (!ch->isOperator(*client))
	{
		client->setSendData(ERR_CHANOPRIVSNEEDED(channelName) + CRLF + CRLF);
		return ;
	}

	// 초대
	cl->addInvited(ch);
	ch->addInviteList(cl->getNick());
	cl->setSendData(RPL_INVITE(client->getNick(), target, channelName) + CRLF);
}