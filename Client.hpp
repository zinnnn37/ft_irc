/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minjinki <minjinki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/11 12:53:46 by minjinki          #+#    #+#             */
/*   Updated: 2023/11/19 12:33:35 by minjinki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <set>

class	Channel;
class	Server;

class	Client
{
	private:
		Server	&_server;

		std::string	_nick;
		std::string	_userName;
		std::string	_realName;

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