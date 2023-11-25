/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minjinki <minjinki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/25 10:51:07 by minjinki          #+#    #+#             */
/*   Updated: 2023/11/25 14:55:03 by minjinki         ###   ########.fr       */
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

Command::Command( Server *server )
	: _server(server)
{
}

Command::~Command() {}

Server	*Command::_getServer() const
{
	return (this->_server);
}


// NICK 명령어 처리
std::string Command::nick(Client *client, std::istringstream &buffer_stream)
{
	std::string 		name;
	std::string 		pre_nick;
	std::string			pre_prefix;
	std::string 		response;

	if (!(buffer_stream >> name)){
		response += "431 "+ client->getNick() + " :Nickname not given";
	}
	else{
		if (this->_server->isClient(name))
			response += "433 "+name +" "+name+" :Nickname is already in use";
		else{
			pre_nick = client->getNick();
			pre_prefix = client->getPrefix();
			client->setNick(name);
			this->_server->changeChannelNick(*client, pre_nick, pre_prefix);
			
			client->setSendData("NICK :" + name + "\n\r\n");
			std::cout << "< Client " << client->getSocket() << " > nickname setted: " << name << std::endl;
		}
	}
	return response;
}