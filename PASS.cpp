/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PASS.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minjinki <minjinki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/25 10:51:45 by minjinki          #+#    #+#             */
/*   Updated: 2023/11/25 12:22:38 by minjinki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"
#include "Client.hpp"
#include "Define.hpp"

void	Command::pass( Client *client, std::istringstream &iss )
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

	if (password != this->_getServer()->getPassword())
	{
		client->setSendData(ERR_PASSWDMISMATCH());
		client->appendSendData(CRLF);
		return ;
	}

	client->setRegistered(true);

	std::cout << "< Client " << client->getSocket() << " > command success" << std::endl;
}