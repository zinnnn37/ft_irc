/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minjinki <minjinki@student.42.kr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/11 12:53:48 by minjinki          #+#    #+#             */
/*   Updated: 2023/12/05 16:36:01 by minjinki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <iostream>
#include <sstream>
#include <string>

#include "Server.hpp"
#include "Define.hpp"

class	Client;
class	Server;
class	Channel;

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
		void	_removeCRLF( std::string &str );
		void	_clientLeaveChannel( Server *server, Client *client, Channel *channel );

		std::string _processJoinChannels(Server *server, Client &client, const std::string &channelName, const std::string &accessKey, std::string &result);
		std::string _clientJoinChannel(Server *server, Client &client, std::string &ch_name, std::string &key);

	public:
		// singleton
		static Command	&getInstance();

		// commands
		void	invite( Server *server, Client *client, std::istringstream &iss );
		void 	join( Server *server, Client *client, std::istringstream &iss );
		void	kick( Server *server, Client *client, std::istringstream &iss );
		void	mode( Server *server, Client *client, std::istringstream &iss );
		void	nick( Server *server, Client *client, std::istringstream &iss );
		void	part( Server *server, Client *client, std::istringstream &iss );
		void	pass( Server *server, Client *client, std::istringstream &iss );
		void	privmsg( Server *server, Client *client, std::istringstream &iss );
		void	topic( Server *server, Client *client, std::istringstream &iss );
		void	user( Client *client, std::istringstream &iss );
		void	quit( Client *client, std::istringstream &iss );
};

#endif