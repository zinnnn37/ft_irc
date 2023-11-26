/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cmd_USER.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minjinki <minjinki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/26 10:10:07 by minjinki          #+#    #+#             */
/*   Updated: 2023/11/26 11:11:11 by minjinki         ###   ########.fr       */
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
		std::cout << i-1 << line << std::endl;
	}
}
void	Command::user( Client *client, std::istringstream &iss )
{
	std::string	line;
	std::string	split[4];

	this->_split(client, iss, split);
	std::cout << std::endl;

	for (int i = 0; i < 4; i++)
		std::cout << i << " " << split[i] << std::endl;

	if (split[0].empty() || split[1].empty() || split[2].empty() || split[3].empty())
	{
		client->setSendData(ERR_NEEDMOREPARAMS(std::string("USER")) + CRLF);
		return ;
	}
}
