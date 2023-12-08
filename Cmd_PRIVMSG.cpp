/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cmd_PRIVMSG.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minjinki <minjinki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/26 15:09:18 by minjinki          #+#    #+#             */
/*   Updated: 2023/12/08 22:08:05 by minjinki         ###   ########.fr       */
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
		client->setSendData(ERR_NEEDMOREPARAMS(client->getNick(), std::string("PRIVMSG")));
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
	std::cout << "[ SERVER ] PRIVMSG to Channel " << message << std::endl;

	Channel	*channel;
	std::string	prefix;

	// channel이 존재하는지 확인
	channel = server->getChannel(target);
	if (!channel)
	{
		client->setSendData(ERR_NOSUCHCHANNEL(client->getNick(), target) + CRLF);
		return ;
	}

	// client가 channel에 속해있는지 확인
	if (!channel->isClient(client->getNick()))
	{
		std::cout << "channel name: " << channel->getName() << "\n";
		std::cout << "client  name: " << client->getNick() << "\n";
		client->setSendData(ERR_CANNOTSENDTOCHAN(client->getNick(), target) + CRLF);
		return ;
	}

	// in the channel
	ClientSet			clients = channel->getClients();
	ClientSet::iterator	it = clients.begin();

	for (; it != clients.end(); it++)
	{
		if ((*it)->getNick() == client->getNick())
			continue ;

		(*it)->setSendData(RPL_PRIVMSG(client->getPrefix(), target, message) + "\n");
	}
}

void	Command::_sendToClient(
	Server *server, Client *client,
	std::string &target, std::string &message )
{
	std::cout << "\n[ SERVER ] PRIVMSG to Client" << message << std::endl;

	Client	*targetClient;

	if (!server->isClient(target))
	{
		client->setSendData(ERR_NOSUCHNICK(client->getNick(), target) + CRLF);
		return ;
	}

	targetClient = server->getClient(target);
	targetClient->setSendData(
		RPL_PRIVMSG(client->getPrefix(), target, message) + "\n");
}