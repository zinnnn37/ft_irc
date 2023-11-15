/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minjinki <minjinki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/11 12:53:48 by minjinki          #+#    #+#             */
/*   Updated: 2023/11/11 13:48:52 by minjinki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMAND_HPP
#define COMMAND_HPP

#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"

class	Server;
class	Client;
class	Channel;

class Command
{
	private:
		std::string	_cmd;

		Command();
		Command( const Command &c );

	public:
		Command( const std::string &cmd );
		~Command();

		// commands
};

#endif