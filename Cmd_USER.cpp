/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cmd_USER.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minjinki <minjinki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/26 10:10:07 by minjinki          #+#    #+#             */
/*   Updated: 2023/12/08 22:08:33 by minjinki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"

void	Command::_split( Client *client, std::istringstream &iss, std::string *split )
{
	int			i = 0;
	std::string	line;

	while (i < 4)
	{
		if (!(iss >> line))
		{
			client->setSendData(ERR_NEEDMOREPARAMS(client->getNick(), std::string("USER")) + CRLF);
			return ;
		}

		split[i++] = line;
	}

	if (split[3][0] == ':')
	{
		split[3] = split[3].substr(1);

		if (split[3].length() == 0)
			iss >> split[3];
	}
	std::cout << "======== real name : " << split[3] << std::endl;
}

void	Command::user( Client *client, std::istringstream &iss )
{
	// user <닉네임> <사용자 이름> <호스트 이름> :<실제 이름>

	std::string	line;
	std::string	split[4];

	this->_split(client, iss, split);
	std::cout << std::endl;

	if (split[0].empty() || split[1].empty() || split[2].empty() || split[3].empty())
	{
		client->setSendData(ERR_NEEDMOREPARAMS(client->getNick(), std::string("USER")) + CRLF);
		return ;
	}

	client->setUserName(split[0]);
	client->setHostName(split[1]);
	client->setServerName(split[2]);
	client->setRealName(split[3]);

	std::cout << "< Client " << client->getSocket() << " > user information set" << std::endl;

	client->setSendData(RPL_WELCOME(client->getNick()) + CRLF);
	client->appendSendData(RPL_YOURHOST(client->getHostName()) + CRLF);
}
