/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minjinki <minjinki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/11 12:53:48 by minjinki          #+#    #+#             */
/*   Updated: 2023/11/29 12:54:56 by minjinki         ###   ########.fr       */
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
		Command();
		Command( const Command &c );
		~Command();

		Command	&operator=( const Command &c );

		void	_split( Client *client, std::istringstream &iss, std::string *split );
		void	_sendToChannel( Server *server, Client *client, std::string &target, std::string &message );
		void	_sendToClient( Server *server, Client *client, std::string &target, std::string &message );
		
		std::string _processJoinChannels(Server *server, Client &client, const std::string &channelName, const std::string &accessKey, std::string &result);
		std::string _clientJoinChannel(Server *server, Client &client, std::string &ch_name, std::string &key);
	
	public:
		// singleton
		static Command	&getInstance();

		// commands
		void	pass( Server *server, Client *client, std::istringstream &iss );
		void	nick( Server *server, Client *client, std::istringstream &iss );
		void	user( Client *client, std::istringstream &iss );
		void	privmsg( Server *server, Client *client, std::istringstream &iss );
		void 	join( Server *server, Client *client, std::istringstream &iss );
		void	invite( Client *client, std::istringstream &iss );
};

#endif