/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minjinki <minjinki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/15 11:09:10 by minjinki          #+#    #+#             */
/*   Updated: 2023/11/15 16:56:02 by minjinki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server() {}

Server::Server( const Server &s )
{
	(void)s;
}

Server::Server( int port, std::string password )
	: _port(port), _password(password)
{
	(void)_port;
	(void)_password;
}

Server::~Server() {}

Server	&Server::operator=( const Server &s )
{
	(void)s;
	return (*this);
}

void	Server::_kqueue()
{
	// 커널에 새로운 이벤트 대기열 생성 후 fd 반환
	this->_kq = kqueue();
	if (this->_kq == -1)
	{
		close(this->_serverSoc);
		throw kqueueException();
	}

	struct kevent	kev;

	// EV_SET: 이벤트를 설정하는 매크로
	EV_SET(&kev, this->_serverSoc, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	/*
	EVFILT_READ: ident의 fd를 모니터링하고
				 data를 읽을 수 있는 경우 event return
	EV_ADD:	kqueue에 event 등록
			식별자(ident, filter)가 같은 event를 재등록하는 경우 덮어쓰기
			등록된 event는 자동으로 활성화
	EV_ENABLE: kevent()가 event를 return할 수 있도록 활성화
	*/
	this->_changeList.push_back(kev);
}

void	Server::_init()
{
	// 소켓 생성
	this->_serverSoc = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_serverSoc == -1)
		throw socketException();

	// 소켓 설정
	int	opt = 1;
	if (setsockopt(this->_serverSoc, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
		throw socketException();
	/*
	SOL_SOCKET: 소켓 옵션 레벨
	-> SO_REUSEADDR: time-wait 상태의 소켓의 포트 번호도 새로운 소켓에 할당할 수 있게 함
	opt가 1이므로 SO_REUSEADDR 옵션 활성화
	*/

	// bind
	struct	sockaddr_in	sockAddr;
	
	bzero(&sockAddr, sizeof(sockAddr));
	sockAddr.sin_family = AF_INET;	// IPv4
	sockAddr.sin_port = htons(this->_port);	// port를 network byte order로 변환
	sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);	// INADDR_ANY: 서버의 IP주소를 자동으로 찾아서 대입

	// 로컬 주소를 소켓과 연결
	if (bind(this->_serverSoc, (struct sockaddr *)&sockAddr, sizeof(sockAddr)))
	{
		close(this->_serverSoc);
		throw bindException();
	}

	// 연결 요청 대기
	if (listen(this->_serverSoc, 128))
	{
		close(this->_serverSoc);
		throw listenException();
	}

	// set non-blocking
	fcntl(this->_serverSoc, F_SETFL, O_NONBLOCK);

	this->_kqueue();
}

void	Server::run()
{
	this->_init();

	int				newEv;
	struct kevent	*kev;

	while (1)
	{
		newEv = kevent(this->_kq,
				&this->_changeList[0], this->_changeList.size(),
				this->_eventList, 1024, NULL);

		if (newEv == -1)
		{
			// client 처리
			close(this->_serverSoc);
			throw keventException();
		}

		this->_changeList.clear();

		for (int i = 0; i < newEv; i++)
		{
			kev = &this->_changeList[i];

			// deal error

			// if read is available

			// if write is available
		}
	}

	// close socket
	// std::cout << close(this->_serverSoc) << std::endl;
}

const char	*Server::socketException::what() const throw()
{
	return ("Exception: socket error");
}

const char	*Server::bindException::what() const throw()
{
	return ("Exception: bind error");
}

const char	*Server::listenException::what() const throw()
{
	return ("Exception: listen error");
}

const char	*Server::kqueueException::what() const throw()
{
	return ("Exception: kqueue error");
}

const char	*Server::keventException::what() const throw()
{
	return ("Exception: kevent error");
}
