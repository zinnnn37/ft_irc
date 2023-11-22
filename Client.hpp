/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minjinki <minjinki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/11 12:53:46 by minjinki          #+#    #+#             */
/*   Updated: 2023/11/22 15:34:08 by minjinki         ###   ########.fr       */
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
		static int	_clientNum;
		bool		_isRegistered;
		bool		_isClosed;
		int			_clientSoc;
		std::string	_addr;

		std::string	_nick;
		std::string	_userName;

		std::string	_preCmd;
		std::string	_buf;

		std::set<Channal *>	_joinedChannels;
		std::set<Channal *>	_invited;

		Client();
		Client( const Client &c );

		Client	&operator=( const Client &c );

	public:
		Client( int socket, std::string addr );
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
		std::string	getBuf() const;
		void		setBuf( std::string &buf );
		std::string	getPreCmd() const;
		void		clearBuf();
		void		appendBuf( std::string buf );

		void		disconnectClientFromChannal();
};

#endif