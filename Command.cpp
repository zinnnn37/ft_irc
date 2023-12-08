/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minjinki <minjinki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/25 10:51:07 by minjinki          #+#    #+#             */
/*   Updated: 2023/12/08 22:08:41 by minjinki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"
#include "Client.hpp"
#include "Define.hpp"

class	Server;

Command::Command() {}

Command::Command( const Command &c )
{
	(void)c;
}

Command::~Command() {}

Command	&Command::operator=( const Command &c )
{
	(void)c;
	return (*this);
}

Command	&Command::getInstance()
{
	static Command	instance;

	return (instance);
}

// NICK 명령어 처리
void Command::nick( Server *server, Client *client, std::istringstream &buffer_stream )
{
	std::string 		name;
	std::string 		pre_nick;
	std::string			pre_prefix;

	if (!(buffer_stream >> name))
		client->setSendData(ERR_NONICKNAMEGIVEN(client->getNick()));
	else
	{
		if (server->isClient(name))
			client->setSendData(ERR_NICKNAMEINUSE(client->getNick(), name) + CRLF);
		else
		{
			pre_nick = client->getNick();
			pre_prefix = client->getPrefix();
			client->setNick(name);
			server->changeChannelNick(*client, pre_nick, pre_prefix);
			
			client->setSendData("NICK :" + name + "\n\r\n");
			std::cout << "< Client " << client->getSocket() << " > nickname setted: " << name << std::endl;
		}
	}
}

void	Command::_removeCRLF( std::string &str )
{
	size_t	pos;

	pos = str.find('\r');
	if (pos != std::string::npos)
		str.erase(pos, 1);
	pos = str.find('\n');
	if (pos != std::string::npos)
		str.erase(pos, 1);
}
