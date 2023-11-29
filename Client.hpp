/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minjinki <minjinki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/11 12:53:46 by minjinki          #+#    #+#             */
/*   Updated: 2023/11/29 15:42:08 by minjinki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Command.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include "Define.hpp"

#include <iostream>
#include <unistd.h>
#include <string>
#include <set>
#include <map>

class Channel;


class	Client
{
	private:
		static int	_clientNum;
		bool		_isRegistered;
		bool		_isClosed;
		int			_clientSoc;
		std::string	_addr;

		std::string	_nick;
		std::string	_userName;
		std::string	_realName;
		std::string	_hostName;
		std::string	_serverName;

		std::string	_buf; 		// server -> client;
		std::string	_sendData;  // client -> server

		std::map<std::string, Channel *>	_joinedChannels;
		std::set<Channel *>	_invited;


	public:
		Client( int socket, std::string addr );
		Client();
		Client( const Client &c );

		Client	&operator=( const Client &c );
		~Client();

		bool	isRegistered() const;
		bool	isClosed() const;
		void	setRegistered( bool isRegistered );
		void	setClosed( bool isClosed );

		int			getSocket() const;
		std::string	getAddr() const;
		std::string	getNick() const;
		void		setNick( const std::string &nick );
		std::string	getUserName() const;
		void		setUserName( const std::string &userName );
		std::string	getRealName() const;
		void		setRealName( const std::string &realName );
		std::string	getHostName() const;
		void		setHostName( const std::string &hostName );
		std::string	getServerName() const;
		void		setServerName( const std::string &serverName );
		std::string	getBuf() const;
		void		setBuf( std::string &buf );
		void		setBuf( std::string buf );
		void		clearBuf();
		void		appendBuf( std::string buf );
		void		setSendData( std::string sendData );
		std::string	getSendData() const;
		void		appendSendData( std::string sendData );
		void		clearSendData();

		void		disconnectClientFromChannel();
		
		std::string getPrefix();
		ChannelMap	getChannels();
		void 		joinChannel(Channel *channel);

		ChannelMap	&getJoinedChannel();
		Channel		*findJoinedChannel( std::string &channelName );

		ChannelSet	&getInvited();
		void		addInvited( Channel *channel );
};

#endif