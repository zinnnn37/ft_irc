/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minjinki <minjinki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/15 11:09:10 by minjinki          #+#    #+#             */
/*   Updated: 2023/11/26 17:24:00 by minjinki         ###   ########.fr       */
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

	(void)client;

	if (cmd == "PASS")
		command.pass(this, client, ss);
	else if (client->isRegistered() == false)
		client->setSendData(ERR_NOTREGISTERED() + std::string(CRLF));
	else if (cmd == "NICK")
		command.nick(this, client, ss);
	else if (cmd == "JOIN")
		this->handleJoin(*client, ss);
	else if (cmd == "USER")
		command.user(client, ss);
	else if (cmd == "PRIVMSG")
		command.privmsg(this, client, ss);
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
	(void)before;
	std::string ch_name;
	std::map<std::string, Channel> channels = client.getChannels();
	std::set<int> c_sockets;
	// int auth;

	for (std::map<std::string, Channel>::iterator m_it = channels.begin(); m_it != channels.end(); m_it++)
	{
				// 서버에 있는 channels 
		ch_name = m_it->second.getName();
		// auth = (this->_channels[ch_name])->getAuth()[before];
		// (this->channels[ch_name])->deleteClient(before);
		// (this->channels[ch_name])->joinClient(client, auth);

		// std::map<std::string, Client> users = (this->_channels[ch_name])->getUsers();
		// for (std::map<std::string, Client>::iterator u_it = users.begin(); u_it != users.end(); u_it++)
		// {
		// 	c_sockets.insert(u_it->second.getSocket());
		// }
	}

	for (std::set<int>::iterator s_it = c_sockets.begin(); s_it != c_sockets.end(); s_it++)
	{
		std::string response;
		response += ":" + before_prefix + " NICK :" + client.getNick();
		std::cout << response << "\n";
		// this->send_data[*s_it] += makeCRLF(response);
		// changeEvent(change_list, *s_it, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
	}
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


// 채널 참가 처리 함수
std::string Server::processJoinChannels(Client &client, const std::string &channelName, const std::string &accessKey, std::string &result)
{
    std::vector<std::string> channels;
    std::stringstream channelStream(channelName);
    std::string currentChannel;

    // 채널 이름에서 ','를 기준으로 분리하여 벡터에 추가
    while (std::getline(channelStream, currentChannel, ','))
    {
        currentChannel.erase(std::remove(currentChannel.begin(), currentChannel.end(), '\r'));
        currentChannel.erase(std::remove(currentChannel.begin(), currentChannel.end(), '\n'));
        channels.push_back(currentChannel);
    }

    std::vector<std::string> accessKeys;
    std::stringstream keyStream(accessKey);
    std::string currentKey;
    // 액세스 키에서 ','를 기준으로 분리하여 벡터에 추가
    while (std::getline(keyStream, currentKey, ','))
    {
        currentKey.erase(std::remove(currentKey.begin(), currentKey.end(), '\r'));
        currentKey.erase(std::remove(currentKey.begin(), currentKey.end(), '\n'));
        accessKeys.push_back(currentKey);
    }

    unsigned int index = 0;

    // 채널과 액세스 키에 대한 루프
    for (std::vector<std::string>::iterator it = channels.begin(); it != channels.end(); it++)
    {
        if (index < accessKeys.size()){
			std::string key = accessKeys[index];
            result += clientJoinChannel(client, *it, accessKeys[index]);
		}
        else{
			std::string key = "";
            result += clientJoinChannel(client, *it, key);

		}
        index++;
    }
	return result;
}


// JOIN 명령어 처리
std::string Server::handleJoin(Client &client, std::istringstream &_bufferStream)
{
    // 클라이언트가 특정 채널에 참가하려는 경우를 처리하는 함수
    // 채널에 참가하는 동작을 수행하고, 결과를 반환
    // 결과는 문자열로 반환되어 클라이언트에게 전달

    std::string result; // 결과를 담을 문자열

    std::string channelName;
    std::string accessKey;

    _bufferStream >> channelName; // 버퍼에서 채널 이름 읽기
    _bufferStream >> accessKey;    // 버퍼에서 액세스 키 읽기

    // JOIN 명령에 필요한 매개변수가 충분하지 않을 경우 에러 메시지 생성
    if (channelName.empty())
    {
        result += "461 " + client.getNick() + " JOIN :Not enough parameters";
        return result;
    }

    // 클라이언트가 최대 채널 수에 가입했을 경우 에러 메시지 생성
    if (client.getChannels().size() > 10)
    {
        result += "405 " + client.getNick() + " " + channelName + " :You have joined too many channels";
        return result;
    }

    // 채널 이름이 '#'로 시작하지 않으면 '#'을 추가
    if (channelName[0] != '#')
        channelName = "#" + channelName;

    // 채널과 액세스 키에 대한 처리를 담당하는 새로운 함수 호출
	result = processJoinChannels(client, channelName, accessKey, result);
    return result; // 최종 결과 반환
}

Channel *Server::createChannel(std::string ch_name, std::string key, Client &client){
	Channel *newchannel = new Channel(key, client);
	this->_channels[ch_name] = newchannel;
	std::cout << "create channel: " << ch_name << std::endl;
	return newchannel;
}

// ### /**MODE (-i, -t, -k, -o, -l) :**  op권한이 있으면, +-로 각종 설정 기능 사용 가능

//  /mode #채널명 +i (초대한 사람만 들어올수 있게함)
//  /mode #채널명 +t (op만 TOPIC을 변경할 수 있게 함)
//  /mode #채널명 +k 비밀번호 (채널에 비밀번호를 설정)
//  /mode #채널명 +o 닉네임 (op권한을 줌)
//  /mode #채널명 +l number (number 만큼 입장가능 인원을 제한함)

// std::

std::string Server::clientJoinChannel(Client &client, std::string &ch_name, std::string &key)
{
	std::string 	response;
	Channel 		*p_channel;


	if (this->_channels.find(ch_name) != this->_channels.end())
		p_channel = this->_channels[ch_name]; // 체널이 있는 경우
	else
		p_channel = createChannel(ch_name, key, client); // 체널이 없는 경우

	if ((!p_channel->getPassword().empty() && key.empty())  // 비밀 번호 존재하는데 client 쪽의 비밀번호가 없는 경우
		|| (!p_channel->getPassword().empty() && key != p_channel->getPassword()) // 비밀 번호가 다른 경우
		|| (p_channel->getPassword().empty() && !key.empty())) // 비밀 번호가 없는데 client가 비밀번호를 보낸 경우 
	{
		std::string response;
		response += "475 " + client.getNick() + " " + ch_name + " :Cannot join channel (+k)";
		return response;
	}
	if (p_channel->getInviteMode() && !p_channel->checkInvite(client.getNick()))
	{
		std::string response;
		response += "473 " + client.getNick() + " " + ch_name + " :Cannot join channel (+i)";
		return response;
	}

	try
	{
		// 사용자 목록을 담을 빈 문자열 초기화
		// 채널의 사용자 목록을 얻어옴
		std::string s_users = "";
		std::set<Client *> users = p_channel->getClients();

		// 클라이언트가 사용자 목록에 없으면 채널에 가입
		if (users.find(&client) == users.end()) {
			// p_channel->joinClient(client, COMMON);
		}

		// 클라이언트를 채널에 가입시킴
		client.joinChannel(p_channel);

		// 채널의 모든 사용자에 대한 루프
		for (std::set<Client *>::iterator it = users.begin(); it != users.end(); ++it)
			s_users.append((*it)->getNick() + " "); // 사용자 목록에 사용자 이름을 추가

		// 채널 가입 메시지를 브로드캐스트
		broadcast(ch_name, RPL_JOIN(client.getPrefix(), ch_name));


		// 채널의 토픽이 비어있지 않으면 토픽과 관련된 응답을 추가
		// if (!p_channel->getTopic().empty())
		// {
			// "332 " + user + " " + channel + " " + topic
			// "333 " + user + " " + channel + " " + nick + " " + setat
			// response += makeCRLF(RPL_TOPIC(client.getUsername(), ch_name, p_channel->getTopic()));
			// response += makeCRLF(RPL_TOPICWHOTIME(client.getUsername(), ch_name, p_channel->getTopicUser(), p_channel->getTopicTime()));
		// }

		// 사용자 목록에 대한 응답을 추가
		// response += makeCRLF(RPL_NAMREPLY(client.getNickname(), '=', ch_name, s_users));

		// 사용자 목록의 끝에 대한 응답을 추가
		// response += makeCRLF(RPL_ENDOFNAMES(client.getNickname(), ch_name));
	}
	catch (const std::exception &e)
	{
		std::cout << "EXCEPTION!!" << std::endl;
		// 예외가 발생한 경우 (예: 채널에서 사용자가 차단된 경우)
		// 차단된 채널에 대한 에러 메시지를 응답에 추가
		// response += makeCRLF(ERR_BANNEDFROMCHAN(client.getNickname(), ch_name));
	}

	return response;
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
		int c_socket = (*u_it)->getSocket();
		(*u_it)->appendSendData(makeCRLF(msg));
		changeEvent(_changeList, c_socket, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
		std::cout << "< Client " << c_socket << " > joined " << channel_name << std::endl;
	}
}


/**********************************************************
 * minjinki ***********************************************
 **********************************************************/

Channel	*Server::getChannel( std::string channelName )
{
	ChannelMap::iterator	it;

	for (it = this->_channels.begin(); it != this->_channels.end(); it++)
	{
		std::cout << "\nchannelName: " << channelName << std::endl;
		std::cout << "it->first: " << it->first << std::endl;
		if (it->first == channelName)
			return (it->second);
	}
	return (NULL);
}