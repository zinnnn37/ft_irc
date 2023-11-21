/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minjinki <minjinki@student.42.kr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/15 11:09:10 by minjinki          #+#    #+#             */
/*   Updated: 2023/11/21 13:32:41 by minjinki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server() {}

Server::Server( const Server &s )
{
	(void)s;
}

Server::Server( int port, std::string password )
	: _port(port), _serverSoc(FD_DEFAULT), _password(password), _kq(FD_DEFAULT)
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

void	Server::_setEvent( int socket, int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data, void *udata )
{
	struct kevent	kev;

	// EV_SET: 이벤트를 설정하는 매크로
	EV_SET(&kev, socket, filter, flags, fflags, data, udata);
	this->_changeList.push_back(kev);
}

void	Server::_init()
{
	// 소켓 생성
	this->_serverSoc = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_serverSoc == -1)
		throw std::runtime_error("Exception: socket error");

	// 소켓 설정
	int	opt = 1;
	if (setsockopt(this->_serverSoc, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
		throw std::runtime_error("Exception: socket error");
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
		throw std::runtime_error("Exception: bind error");
	}

	// 연결 요청 대기
	if (listen(this->_serverSoc, 10))
	{
		close(this->_serverSoc);
		throw std::runtime_error("Exception: listen error");
	}

	// set non-blocking
	this->_setNonBlock(this->_serverSoc);

	// 커널에 새로운 이벤트 대기열 생성 후 fd 반환
	this->_kq = kqueue();
	if (this->_kq == -1)
	{
		close(this->_serverSoc);
		throw std::runtime_error("Exception: kqueue error");
	}

	this->_setEvent(this->_serverSoc, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	/*
	EVFILT_READ: ident의 fd를 모니터링하고
				 data를 읽을 수 있는 경우 event return
	EV_ADD:	kqueue에 event 등록
			식별자(ident, filter)가 같은 event를 재등록하는 경우 덮어쓰기
			등록된 event는 자동으로 활성화
	EV_ENABLE: kevent()가 event를 return할 수 있도록 활성화
	*/
}

void	Server::run()
{
	this->_init();

	int				newEv;

	while (1)
	{
		newEv = kevent(this->_kq,
				&this->_changeList[0], this->_changeList.size(),
				this->_eventList, 128, NULL);

		if (newEv == -1)
			this->_exit("Exception: kevent error");

		this->_changeList.clear();

		for (int i = 0; i < newEv; i++)
			this->_handleEvent(this->_eventList[i]);
	}
}

void	Server::_handleEvent( struct kevent &kev )
{
	(void)kev;
	if (kev.flags & EV_ERROR)
	{
		// Server Error
		if (kev.ident == (uintptr_t)this->_serverSoc)
		{
			this->_free();
			throw std::runtime_error("Exception: server error");
		}
		// Client Error
		else
		{
			std::cerr << "Exception: Client error" << std::endl;
			this->_disconnectClient(kev.ident);
		}
	}
	//Able to read
	else if (kev.filter == EVFILT_READ)
	{
		// New Client connects to server
		if (kev.ident == (uintptr_t)this->_serverSoc)
			this->_acceptNewClient();
		// Connected Client sends message
		else
			this->_readDataFromClient(kev);
	}
	// Able to write
	else if (kev.filter == EVFILT_WRITE)
		this->_sendDataToClient(kev.ident);
}

void	Server::_disconnectClient( uintptr_t ident )
{
	// ClientMap::iterator	it = this->_clients.find(ident);
	// Client	*client = it->second;

	// if (it == this->_clients.end())
	// 	return ;
	
	// this->_clients.erase(ident);

	// ChannalSet	&joinedChannals = client->getJoinedChannals();
	// for (ChannalVec::iterator it = joinedChannals.begin(); it != joinedChannals.end(); it++)
	// {
	// 	(*it)->removeClient(client);
	// 	// 클라이언트가 채널에 남아있는 유일한 클라이언트였다면 채널 삭제
	// }
	// -> Client 멤버함수로 옮기는 게 깔끔할 듯

	// delete client;	// 소멸자에서 멤버변수 정리

	// std::cout << "[ SERVER ] Client disconnected" << std::endl;
}

void	Server::_acceptNewClient()
{
	int					clientSoc;
	struct sockaddr_in	clientAddr;
	socklen_t			clientAddrSize = sizeof(clientAddr);

	bzero(&clientAddr, sizeof(clientAddr));

	clientSoc = accept(this->_serverSoc, (struct sockaddr *)&clientAddr, &clientAddrSize);
	if (clientSoc == RET_ERROR)
	{
		std::cerr << "Error: accept error" << std::endl;
		return ;
	}

	// if (clientSoc >= MAX_CLIENT)
	// {
	// 	std::cout << "[ SERVER ] Has max Client" << std::endl;
	// 	close(clientSoc);
	// 	return ;
	// }
	
	std::string	addr(inet_ntoa(clientAddr.sin_addr));
	// Client *newClient = new Client(clientSoc, addr);
	// this->_clients.insert(std::make_pair(clientSoc, newClient));

	std::cout << "[ SERVER ] New Client connected" << std::endl;

	// set non-blocking
	this->_setNonBlock(clientSoc);
	this->_setEvent(clientSoc, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
}

void	Server::_readDataFromClient( struct kevent &kev )
{
	char	buf[1024];
	// Client *client = this->_clients[kev.ident]; // find로 찾기
	ssize_t	byte;

	bzero(buf, sizeof(buf));

	byte = recv(kev.ident, buf, sizeof(buf), 0);
	if (byte <= 0)
	{
		if (byte < ERROR && errno == EAGAIN)
			return ;
		std::cerr << "Error: recv error" << std::endl;
		// broadcast to all client
		this->_disconnectClient(kev.ident);
		return ;
	}
	buf[byte] = '\0';
	client->addBuffer(buf);
	this->_handleInput(client);
}

void	Server::_handleInput( Client *client )
{
	size_t		crlf;
	std::string	buf;
	std::string	line;

	while (true)
	{
		if (client->isClosed())
			break ;

		buf = client->getBuffer();
		crlf = buf.find(CRLF);

		if (crlf != std::string::npos)
		{
			line = buf.substr(0, crlf + 1);
			std::cout << "[ SERVER ] " << line << std::endl;
		}
		else
		{
			Message	msg(buf.substr(0, crlf));
			client->setBuffer(buf.substr(crlf + 1));
		}
	}
}

void	Server::_sendDataToClient( uintptr_t ident )
{
	int					byte;
	std::string			rplbuf;
	ClientMap::iterator	it = this->_clients.find(ident);
	Client				*client = it->second;

	if (it == this->_clients.end()) return ;

	rplbuf = client->getRplBuf();
	if (rplbuf) return ;

	byte = send(ident, rplbuf.c_str(), rplbuf.length(), 0);
	if (byte == ERROR)
	{
		std::cout << "[ SERVER ] Client send error" << std::endl;
		// broadcast
		this->_disconnectClient(ident);
	}
	else
	{
		client->setRplBuf(rplbuf.substr(byte));
		if (client->getRplBuf().empty())
			this->_disconnectClient(ident);
	}
}
