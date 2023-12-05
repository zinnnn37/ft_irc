/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cnd_QUIT.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minjinki <minjinki@student.42.kr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/05 16:35:17 by minjinki          #+#    #+#             */
/*   Updated: 2023/12/05 16:40:56 by minjinki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"

void	Command::quit( Client *client, std::istringstream &iss )
{
	std::string		message;

	std::getline(iss, message);

	if (message.empty())
		message = "Client quit";
	else
	{
		this->_removeCRLF(message);
		message.substr(1);
	}

	client->setSendData(message + CRLF);
}