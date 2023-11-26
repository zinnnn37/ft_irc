/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cmd_PRIVMSG.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minjinki <minjinki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/26 15:09:18 by minjinki          #+#    #+#             */
/*   Updated: 2023/11/26 17:25:35 by minjinki         ###   ########.fr       */
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

	std::cout << "privmsg" << std::endl;
}

void	Command::_sendToChannel(
	Server *server, Client *client,
	std::string &target, std::string &message )
{
	Channel	*channel;
	std::string	prefix;

	channel = server->getChannel(target);
	if (!channel)
	{
		client->setSendData(ERR_NOSUCHCHANNEL(target));
		client->appendSendData(CRLF);
		return ;
	}

	ClientSet	clients = channel->getClients();
	// not in the channel
	if (clients.find(client) == clients.end())
	{
		client->setSendData(ERR_CANNOTSENDTOCHAN(target));
		client->appendSendData(CRLF);
		return ;
	}

	// in the channel
	ClientSet::iterator	it;

	for (it = clients.begin(); it != clients.end(); it++)
	{
		(*it)->setSendData(RPL_PRIVMSG(client->getPrefix(), target, message));
		(*it)->appendSendData(CRLF);
	}
}

void	Command::_sendToClient(
	Server *server, Client *client,
	std::string &target, std::string &message )
{
	(void)server;
	(void)client;
	(void)target;
	(void)message;
}