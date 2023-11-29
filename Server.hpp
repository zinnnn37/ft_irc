/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minjinki <minjinki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/11 12:53:51 by minjinki          #+#    #+#             */
/*   Updated: 2023/11/29 16:56:49 by minjinki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <sstream>
#include <cstdint>
#include <string>
#include <list>
#include <vector>
#include <map>
#include <sys/socket.h>
#include <sys/event.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <exception>

#include "Command.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include "Define.hpp"

class	Command;

class Server
{
	private:
		std::string							_serverName;
		std::string							_hostname;

		int									_port;
		int									_serverSoc;
		std::string							_password;

		int									_kq;				// kqueue fd
		struct kevent						_eventList[128];	// 발생한 이벤트 배열
		std::vector<struct kevent>			_changeList;		// kqueue에 등록할 이벤트 리스트

		std::map<int, Client *>				_clients;	// Server에 존재하는 모든 클라이언트, int는 소켓 번호
		std::map<std::string, Channel *>	_channels;	// Server에 존재하는 모든 채널, string은 채널 이름

		Server();
		Server( const Server &s );

		Server &operator=( const Server &s );

		void		_init();
		void		_setEvent( int socket, int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data, void *udata );


		void		_handleEvent( struct kevent &kev );

		void		_acceptNewClient();
		void		_readDataFromClient( struct kevent &kev );
		void		_sendDataToClient( uintptr_t ident );
		void		_disconnectClient( uintptr_t ident );

		void		_handleMsg( Client *client );
		void		_handleCommand( Client *client, std::string line, std::string buf, size_t crlf );

		void		_setNonBlock( int fd );
		void		_free();
		void		_exit( const char *errmsg );


	public:
		Server( int port, std::string password );
		~Server();

		void		run();
		void 		broadcast(std::string &channel_name, const std::string &msg);
		void 		changeEvent(std::vector<struct kevent> &change_list, uintptr_t ident, int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data, void *udata);
		void 		changeChannelNick(Client& client, const std::string& before, const std::string& before_prefix);

		bool 		isClient(const std::string& nickname);
		bool		isChannel(std::string &ch_name);


		std::string	getPassword() const;
		// std::string handleJoin(Client &client, std::istringstream &_bufferStream);
		// std::string processJoinChannels(Client &client, const std::string &channelName, const std::string &accessKey, std::string &result);
		// std::string clientJoinChannel(Client &client, std::string &ch_name, std::string &key);
		std::string makeCRLF(std::string buffer);

		ChannelMap	&getChannels();
		Channel 	*createChannel(std::string ch_name, std::string key, Client &client);
		Channel		*getChannel( std::string channelName );
		Client		*getClient( std::string nick );
};
#endif