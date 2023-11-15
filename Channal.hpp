/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channal.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minjinki <minjinki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/11 12:53:41 by minjinki          #+#    #+#             */
/*   Updated: 2023/11/11 13:42:46 by minjinki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNAL_HPP
#define CHANNAL_HPP

#include "Server.hpp"
#include "Client.hpp"

class	Client;

class	Channal
{
	private:
		bool	_isInviteOnly;
		bool	_isTopicRestricted;

		std::string			_channalName;
		std::string			_topic;
		std::string			_password;

		std::set<Client *>	_clients;
		std::set<Client *>	_operators;

		Channal();
		Channal( const Channal &c );

		Channal	&operator=( const Channal &c );

	public:
		Channal( const std::string &channalName, Client &client );
		~Channal();

		int	addClient( Client &client );
		int	removeClient( Client &client );
		int	addOperator( Client &client );
		int	removeOperator( Client &client );

		int	kickClient( Client &client );

		int	setTopic( Client &client, const std::string &topic );

		int	setMode( Client &client, const std::string &mode );

		int	sendMessage( Client &client, const std::string &message );
}

#endif