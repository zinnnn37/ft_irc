/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minjinki <minjinki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/15 11:09:10 by minjinki          #+#    #+#             */
/*   Updated: 2023/11/25 12:29:37 by minjinki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"

Server::Server() {}

Server::Server( const Server &s )
{
	(void)s;
}

Server::Server( int port, std::string password )
	: _serverName("ircserv"), _port(port), _serverSoc(FD_DEFAULT), _password(password), _kq(FD_DEFAULT), _command(new Command(this))
{
	(void)_port;
	(void)_password;
}

Server::~Server()
{
	delete this->_command;
}

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
	ClientMap::iterator	it = this->_clients.find(ident);
	Client	*client = it->second;

	if (it == this->_clients.end())
		return ;
	
	this->_clients.erase(ident);

	client->disconnectClientFromChannel();

	delete client;

	std::cout << "[ SERVER ] Client disconnected" << std::endl;
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

	if (clientSoc >= MAX_CLIENT)
	{
		std::cout << "[ SERVER ] Has max Client" << std::endl;
		close(clientSoc);
		return ;
	}

	// set non-blocking
	this->_setNonBlock(clientSoc);
	this->_setEvent(clientSoc, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	this->_setEvent(clientSoc, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
	
	std::string	addr(inet_ntoa(clientAddr.sin_addr));
	Client *newClient = new Client(clientSoc, addr);
	this->_clients.insert(std::make_pair(clientSoc, newClient));

	std::cout << "[ SERVER ] New Client connected: " << newClient->getSocket() << std::endl;
}

void	Server::_readDataFromClient( struct kevent &kev )
{
	char				buf[1024];
	ssize_t				byte;
	ClientMap::iterator	it = this->_clients.find(kev.ident);
	Client				*client = it->second;

	if (it == this->_clients.end()) return ;

	bzero(buf, sizeof(buf));

	byte = recv(kev.ident, buf, sizeof(buf), 0);
	if (byte <= 0)
	{
		if (byte <= ERROR && errno == EAGAIN)
			return ;
		std::cerr << "< Client " << kev.ident << " > Error: recv error" << std::endl;
		// broadcast to all client
		this->_disconnectClient(kev.ident);
	}
	else
	{
		buf[byte] = '\0';
		client->appendBuf(buf);
		this->_handleMsg(client);
	}
}

void	Server::_handleMsg( Client *client )
{
	size_t		crlf;
	std::string	buf;
	std::string	line;

	while (true)
	{
		if (client->isClosed())
			break ;

		buf = client->getBuf();
		crlf = buf.find(CRLF);

		if (crlf != std::string::npos)
		{
			line = buf.substr(0, crlf + 2);
			std::cout << "[ SERVER ] message recieved from " << client->getSocket() << ": " << line << std::endl;

			// command 처리
			std::cout << "buf : " << buf << std::endl;
			std::cout << "line: " << line << std::endl;
			this->_handleCommand(client, line, buf, crlf);
			// client->clearBuf();
			// break ;
		}
		else
			break ;
	}
}

void	Server::_handleCommand( Client *client, std::string line, std::string buf, size_t crlf )
{
	std::string			cmd;
	std::string			msg;
	std::istringstream	ss(line);	// space 단위로 문자열 읽기

	ss >> cmd;

	for (std::string::iterator it = cmd.begin(); it != cmd.end(); it++)
		*it = toupper(*it);

	(void)client;

	if (cmd == "PASS")
		this->_command->pass(client, ss);
	// 	case "NICK":
	// 		this->_command.nick(client, ss);
	// 		break ;
		
	// 	case "USER":
	// 		this->_command.user(client, ss);
	// 		break ;
		
	// 	case "JOIN":
	// 		this->_command.join(client, ss);
	// 		break ;
		
	// 	case "PRIVMSG":
	// 		this->_command.privmsg(client, ss);
	// 		break ;
		
	// 	case "KICK":
	// 		this->_command.kick(client, ss);
	// 		break ;

	// 	case "INVITE":
	// 		this->_command.invite(client, ss);
	// 		break ;
		
	// 	case "TOPIC":
	// 		this->_command.topic(client, ss);
	// 		break ;
		
	// 	case "MODE":
	// 		this->_command.mode(client, ss);
	// 		break ;
		
	// 	case "PART":
	// 		this->_command.part(client, ss);
	// 		break ;
		
	// 	case "EXIT"
	// 		this->_command.exit(client, ss);
	// 		break ;
	// }
	// client->appendSend();	// response 받아서 crlf 붙여서 append. 이거를 _sendData에서 Clinet로 보냄
	std::cout << "deal with cmd" << std::endl;
	client->setBuf(buf.substr(crlf + 2));	// crlf 이후 문자열
}

void	Server::_sendDataToClient( uintptr_t ident )
{
	(void)ident;
	int					byte;
	std::string			data;
	ClientMap::iterator	it = this->_clients.find(ident);
	Client					*client = it->second;

	if (it == this->_clients.end()) return ;

	if (client->getSendData().empty()) return ;

	std::cout << "[ SERVER ] message sent: " << client->getSendData()
		<< "< Client " << client->getSocket() << " > " << client->getNick() << std::endl;

	data = client->getSendData();
	byte = send(ident, data.c_str(), data.length(), 0);
	if (byte == RET_ERROR)
	{
		std::cerr << "[ SERVER ] Error: send error" << std::endl;
		this->_disconnectClient(ident);
	}
	else
	{
		client->clearSendData();
		if (client->isClosed())
			this->_disconnectClient(ident);
	}
}

std::string	Server::getPassword() const
{
	return (this->_password);
}
