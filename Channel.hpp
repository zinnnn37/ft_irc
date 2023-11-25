/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minjinki <minjinki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/11 12:53:41 by minjinki          #+#    #+#             */
/*   Updated: 2023/11/25 11:39:46 by minjinki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>

class	Channel
{
	private:
		bool	_isInviteOnly;
		bool	_isTopicRestricted;

		std::string			_channelName;
		std::string			_topic;
		std::string			_password;

		std::set<Client *>	_clients;
		std::set<Client *>	_operators;

		Channel();
		Channel( const Channel &c );

		Channel	&operator=( const Channel &c );

	public:
		Channel( const std::string &channelName, Client &client );
		~Channel();

		int	addClient( Client &client );
		int	removeClient( Client &client );
		int	addOperator( Client &client );
		int	removeOperator( Client &client );

		int	kickClient( Client &client );

		int	setTopic( Client &client, const std::string &topic );

		int	setMode( Client &client, const std::string &mode );

		int	sendMessage( Client &client, const std::string &message );
};

#endif