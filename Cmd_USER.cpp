/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cmd_USER.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minjinki <minjinki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/26 10:10:07 by minjinki          #+#    #+#             */
/*   Updated: 2023/11/26 11:32:13 by minjinki         ###   ########.fr       */
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
			client->setSendData(ERR_NEEDMOREPARAMS(std::string("USER")) + CRLF);
			return ;
		}

		if (line[0] == ':')
		{
			line = line.substr(1);

			if (line.length() == 0)
				continue ;
		}
		split[i++] = line;
	}
}

void	Command::user( Client *client, std::istringstream &iss )
{
	std::string	line;
	std::string	split[4];

	this->_split(client, iss, split);
	std::cout << std::endl;

	// for (int i = 0; i < 4; i++)
	// 	std::cout << i << " " << split[i] << std::endl;

	if (split[0].empty() || split[1].empty() || split[2].empty() || split[3].empty())
	{
		client->setSendData(ERR_NEEDMOREPARAMS(std::string("USER")) + CRLF);
		return ;
	}

	client->setUserName(split[0]);
	client->setHostName(split[1]);
	client->setServerName(split[2]);
	client->setRealName(split[3]);

	// std::cout << client->getNick() << std::endl;
	// std::cout << client->getUserName() << std::endl;
	// std::cout << client->getHostName() << std::endl;
	// std::cout << client->getServerName() << std::endl;
	// std::cout << client->getRealName() << std::endl;

	std::cout << "< Client " << client->getSocket() << " > user information set" << std::endl;

	client->setSendData(RPL_WELCOME(client->getNick()) + CRLF);
	client->appendSendData(RPL_YOURHOST(client->getHostName()) + CRLF);
}
