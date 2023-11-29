/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cmd_PART.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minjinki <minjinki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/29 15:46:06 by minjinki          #+#    #+#             */
/*   Updated: 2023/11/29 15:54:32 by minjinki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"

void	Command::part( Server *server, Client *client, std::istringstream &iss )
{
	(void)server;
	(void)client;

	std::string	channelName;

	std::cout << "===================" << std::endl;
	while (std::getline(iss, channelName, ','))
	{
		if (channelName[0] == ' ')
			channelName = channelName.substr(1);
		std::cout << channelName << std::endl;
	}
}
