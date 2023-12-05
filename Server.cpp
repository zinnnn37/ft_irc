/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minjinki <minjinki@student.42.kr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/15 11:09:10 by minjinki          #+#    #+#             */
/*   Updated: 2023/12/05 16:39:55 by minjinki         ###   ########.fr       */
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
	: _serverName("ircserv"), _port(port), _serverSoc(FD_DEFAULT), _password(password), _kq(FD_DEFAULT)
{
	(void)_port;
	(void)_password;
}

Server::~Server()
{
	ChannelMap::iterator	it = this->_channels.begin();

	// 채널
	for (; it != this->_channels.end(); it++)
	{
		delete it->second;
		it->second = 0;
	}

	// 클라이언트
	ClientMap::iterator	it2 = this->_clients.begin();
	for (; it2 != this->_clients.end(); it2++)
	{
		delete it2->second;
		it2->second = 0;
	}
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
	sockAddr.sin_family = AF_INET;	// 주소체계 구분, IPv4
	sockAddr.sin_port = htons(this->_port);	// port를 network byte order로 변환(16비트로 변환)
	sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	// 호스트 IP 주소
	// INADDR_ANY: 서버의 IP주소를 자동으로 찾아서 대입

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
	// 정수형 IP주소를 문자열로 변환
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
		if (byte < ERROR)
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
	Command				&command = Command::getInstance();

	ss >> cmd;

	for (std::string::iterator it = cmd.begin(); it != cmd.end(); it++)
		*it = toupper(*it);

	// (void)client;

	if (cmd == "PASS")
		command.pass(this, client, ss);
	else if (client->isRegistered() == false)
		client->setSendData(ERR_NOTREGISTERED() + std::string(CRLF));
	else if (cmd == "NICK")
		command.nick(this, client, ss);
	else if (cmd == "JOIN")
		command.join(this, client, ss);
	else if (cmd == "USER")
		command.user(client, ss);
	else if (cmd == "PRIVMSG")
		command.privmsg(this, client, ss);
	else if (cmd == "INVITE")
		command.invite(this, client, ss);
	else if (cmd == "PART")
		command.part(this, client, ss);
	else if (cmd == "MODE")
		command.mode(this, client, ss);
	else if (cmd == "KICK")
		command.kick(this, client, ss);
	else if (cmd == "TOPIC")
		command.topic(this, client, ss);
	else if (cmd == "QUIT")
		command.quit(client, ss);
	else if (cmd == "TERM")	// 서버 종료 용, 제출 시 삭제
		this->_exit("Server terminated");

	client->setBuf(buf.substr(crlf + 2));	// crlf 이후 문자열
}


bool Server::isClient(const std::string& nickname)
{
    ClientMap::iterator it = this->_clients.begin();

	for (; it != this->_clients.end(); it++){
		if (it->second->getNick() == nickname)
			return true;
	}
	return false;
}

void Server::changeChannelNick(Client& client, const std::string& before, const std::string& before_prefix)
{
	std::string	auth;
	std::string	channelName;
	ClientMap	channels = client.getChannels();

	for (ChannelMap::iterator it = channels.begin(); it != channels.end(); it++)
	{
		// 서버에 있는 channels
		channelName = it->second->getName();
		auth = (this->_channels[channelName])->getAuth(before)

		if (auth != "")
		{
			(this->channels[channelName])->removeClient(before);
			(this->channels[channelName])->joinClient(client, auth);

			std::string	res;
			res += ":" + before_prefix + " NICK :" + client.getNick();

			this->broadcast(channelName, res);
		}
	}
}

void	Server::_sendDataToClient( uintptr_t ident )
{
	int					byte;
	std::string			data;
	ClientMap::iterator	it = this->_clients.find(ident);
	Client					*client = it->second;

	if (it == this->_clients.end()) return ;

	if (client->getSendData().empty()) return ;

	std::cout << "[ SERVER ] message sent: " << client->getSendData() << std::endl;

	data = client->getSendData();
	byte = send(ident, data.c_str(), data.size(), 0);
	if (byte == RET_ERROR)
	{
		client->clearSendData();
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


bool Server::isChannel(std::string &ch_name){
	if (this->_channels.find(ch_name) != this->_channels.end())
		return true;
	else
		return false;
	
}


Channel *Server::createChannel( std::string ch_name, std::string key, Client &client){
	Channel *newchannel = new Channel(ch_name, client, key);
	this->_channels[ch_name] = newchannel;
	newchannel->setName(ch_name);
	newchannel->addOperator(client);
	std::cout << "create channel: " << ch_name << std::endl;
	return newchannel;
}

std::string Server::makeCRLF(std::string buffer){
	return buffer += "\r\n";
}

// change_list 에 새 이벤트 추가
void Server::changeEvent(std::vector<struct kevent> &change_list, uintptr_t ident, int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data, void *udata)
{
	(void)change_list;
	struct kevent temp_event;

	// kevent 구조체인 temp_event를 인자들로 설정
	EV_SET(&temp_event, ident, filter, flags, fflags, data, udata);
	// 설정한 이벤트를 kevent 배열에 추가
	this->_changeList.push_back(temp_event);
}

void Server::broadcast(std::string &channel_name, const std::string &msg)
{
	Channel *channel = this->_channels[channel_name];

	std::set<Client *> users = channel->getClients();
	for (std::set<Client *>::iterator u_it = users.begin(); u_it != users.end(); ++u_it)
	{
		int clientSoc = (*u_it)->getSocket();
		(*u_it)->setSendData(msg + CRLF);
		changeEvent(_changeList, clientSoc, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
		std::cout << "< Client " << clientSoc << " >" << std::endl;
	}
}


/**********************************************************
 * minjinki ***********************************************
 **********************************************************/

ChannelMap	&Server::getChannels()
{
	return (this->_channels);
}

Channel	*Server::getChannel( std::string channelName )
{
	ChannelMap::iterator	it;

	for (it = this->_channels.begin(); it != this->_channels.end(); it++){
		// std::cout << "channelName: " << channelName << std::endl;
		// std::cout << "it->first: " << it->first << std::endl;
		if (it->first == channelName)
			return (it->second);
	}

	return (NULL);
}

Client	*Server::getClient( std::string nick ){
	ClientMap::iterator	it = this->_clients.begin();

	for (; it != this->_clients.end(); it++)
	{
		if (it->second->getNick() == nick)
			return (it->second);
	}

	return (NULL);
}