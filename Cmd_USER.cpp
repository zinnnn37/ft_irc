/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cmd_USER.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minjinki <minjinki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/26 10:10:07 by minjinki          #+#    #+#             */
/*   Updated: 2023/11/26 11:00:28 by minjinki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"

void	Command::_split( Client *client, std::istringstream &iss, std::string *split )
{
	int			i = 0;
	std::string	line;
	std::string	user = "USER";

	while (i < 4)
	{
		iss >> line;

		if (line.empty())
		{
			client->setSendData(ERR_NEEDMOREPARAMS(user));
			return ;
		}

		if (line[0] == ':')
		{
			line = line.substr(1);
			split[i++] = line;

			if (line.length() == 0)
			{
				iss >> line;
				split[3] = line;
			}
		}
		split[i++] = line;
	}
}
void	Command::user( Client *client, std::istringstream &iss )
{
	std::string	line;
	std::string	split[4];

	this->_split(client, iss, split);

	
}
