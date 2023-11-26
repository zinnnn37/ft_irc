/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minjinki <minjinki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/11 12:53:48 by minjinki          #+#    #+#             */
/*   Updated: 2023/11/26 09:52:52 by minjinki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <iostream>
#include <sstream>
#include <string>

#include "Server.hpp"

class	Client;
class	Server;

class	Command
{
private:
	Server	*_server;

	Command( const Command &c );

	Server	*_getServer() const;

public:
	Command();
	Command( Server *server );
	~Command();

	// commands
	void	pass(Client *client, std::istringstream &iss);
	void	nick(Client *client, std::istringstream &buffer_stream);

};

#endif