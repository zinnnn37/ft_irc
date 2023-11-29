/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cmd_PRIVMSG.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minjinki <minjinki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/26 15:09:18 by minjinki          #+#    #+#             */
/*   Updated: 2023/11/29 11:03:57 by minjinki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"

void	Command::privmsg( Server *server, Client *client, std::istringstream &iss )
{
	std::string	target;
	std::string	message;

	iss >> target;
	std::getline(iss, message);	// 공백 포함해서 읽어옴

	if (target.empty())
	{
		client->setSendData(ERR_NEEDMOREPARAMS(std::string("PRIVMSG")));
		client->appendSendData(CRLF);
		return ;
	}

	// send message
	if (target[0] == '#' || target[0] == '&')
		this->_sendToChannel(server, client, target, message);
	else
		this->_sendToClient(server, client, target, message);
}

void	Command::_sendToChannel(
	Server *server, Client *client,
	std::string &target, std::string &message )
{
	(void)message;

	Channel	*channel;
	std::string	prefix;

	channel = server->getChannel(target);
	if (!channel)
	{
		client->setSendData(ERR_NOSUCHCHANNEL(target));
		client->appendSendData(CRLF);
		return ;
	}

	if (!channel->isClient(client->getNick()))
	{
		client->setSendData(ERR_CANNOTSENDTOCHAN(target));
		client->appendSendData(CRLF);
		return ;
	}

	// in the channel
	ClientSet			clients = channel->getClients();
	ClientSet::iterator	it = clients.begin();

	for (; it != clients.end(); it++)
	{
		if ((*it)->getNick() == client->getNick())
			continue ;

		(*it)->setSendData(RPL_PRIVMSG(client->getPrefix(), target, message));
		(*it)->appendSendData(CRLF);
	}
}

void	Command::_sendToClient(
	Server *server, Client *client,
	std::string &target, std::string &message )
{
	Client	*targetClient;

	if (!server->isClient(target))
	{
		client->setSendData(ERR_NOSUCHNICK(target));
		client->appendSendData(CRLF);
		return ;
	}

	std::cout << "\n[ SERVER ] PRIVMSG to " << target << " : " << message << std::endl;

	targetClient = server->getClient(target);
	targetClient->setSendData(
		RPL_PRIVMSG(client->getPrefix(), target, message));
	targetClient->appendSendData(CRLF);
}