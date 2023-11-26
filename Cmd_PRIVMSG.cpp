/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cmd_PRIVMSG.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minjinki <minjinki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/26 15:09:18 by minjinki          #+#    #+#             */
/*   Updated: 2023/11/26 15:26:59 by minjinki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"

void	Command::privmsg( Client *client, std::istringstream &iss )
{
	(void)client;
	(void)iss;

	std::cout << "privmsg" << std::endl;
}