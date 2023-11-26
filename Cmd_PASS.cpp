/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cmd_PASS.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minjinki <minjinki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/25 10:51:45 by minjinki          #+#    #+#             */
/*   Updated: 2023/11/26 15:26:17 by minjinki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"
#include "Client.hpp"
#include "Define.hpp"

void	Command::pass( Server *server, Client *client, std::istringstream &iss )
{
	std::string		password;

	if (client->isRegistered())
	{
		client->setSendData(ERR_ALREADYREGISTRED());
		client->appendSendData(CRLF);
		return ;
	}

	iss >> password;

	if (password.empty())
	{
		client->setSendData(ERR_NEEDMOREPARAMS(std::string("PASS")));
		client->appendSendData(CRLF);
		return ;
	}

	if (password != server->getPassword())
	{
		client->setSendData(ERR_PASSWDMISMATCH());
		client->appendSendData(CRLF);
		return ;
	}

	client->setSendData("Password accepted\n\r\n");
	client->setRegistered(true);

	std::cout << "< Client " << client->getSocket() << " > PASS success" << std::endl;
}