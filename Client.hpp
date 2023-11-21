/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minjinki <minjinki@student.42.kr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/11 12:53:46 by minjinki          #+#    #+#             */
/*   Updated: 2023/11/21 13:23:37 by minjinki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <set>
#include "Channal.hpp"

class	Channel;
class	Server;

class	Client
{
	private:
		Server	&_server;

		std::string	_nick;
		std::string	_userName;
		std::string	_realName;

		std::string	_cmdBuf;
		std::string _rplBuf;

		std::set<Channal *>	_joinedChannels;
		std::set<Channal *>	_invited;

		Client();
		Client( const Client &c );

		Client	&operator=( const Client &c );

	public:
		Client( Server &server );
		~Client();

		int	init( int serverfd );

		int	sendMessage( const std::string &message );

		void	addInvited( Channal &channal );
};