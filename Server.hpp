/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minjinki <minjinki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/11 12:53:51 by minjinki          #+#    #+#             */
/*   Updated: 2023/11/18 14:53:18 by minjinki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <cstdint>
#include <string>
#include <list>
#include <vector>
#include <map>
#include <sys/socket.h>
#include <sys/event.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <exception>

// #include "Client.hpp"
// #include "Channal.hpp"
// #include "Command.hpp"
#include "Define.hpp"

class	Channal;
class	Client;

class Server
{
	private:
		int									_port;
		int									_serverSoc;
		std::string							_password;

		int									_kq;				// kqueue fd
		struct kevent						_eventList[128];	// 발생한 이벤트 배열
		std::vector<struct kevent>			_changeList;		// kqueue에 등록할 이벤트 리스트

		// std::map<int, Client *>				_clients;
		// std::map<std::string, Channal *>	_channals;
		// Command								_command; //?

		Server();
		Server( const Server &s );

		Server &operator=( const Server &s );

		void	_init();
		void	_kqueue();
		void	_setEvent( int socket, int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data, void *udata );

		void	_handleEvent( struct kevent &kev );

		void	_acceptNewClient();
		void	_readDataFromClient( uintptr_t ident );
		void	_sendDataToClient( uintptr_t ident );
		void	_disconnectClient( uintptr_t ident );

		void	_free();
		void	_exit( const char *errmsg );

	public:
		Server( int port, std::string password );
		~Server();

		void	run();

		class	socketException : std::exception
		{
			public:
				const char*	what() const throw();
		};

		class	bindException : std::exception
		{
			public:
				const char*	what() const throw();
		};

		class	listenException : std::exception
		{
			public:
				const char*	what() const throw();
		};

		class	kqueueException : std::exception
		{
			public:
				const char*	what() const throw();
		};

		class	keventException : std::exception
		{
			public:
				const char*	what() const throw();
		};

		class	serverException : std::exception
		{
			public:
				const char*	what() const throw();
		};

		class	acceptException : std::exception
		{
			public:
				const char*	what() const throw();
		};
};

#endif