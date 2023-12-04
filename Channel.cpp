#include "Channel.hpp"

// 기본 생성자
Channel::Channel() : _isInviteOnly(false), _isTopicRestricted(false) {
    this->_accessLimit = 1000000;
}

// 복사 생성자
Channel::Channel(const Channel &c)
    : _isInviteOnly(c._isInviteOnly), _isTopicRestricted(c._isTopicRestricted),
      _channelName(c._channelName), _topic(c._topic), _password(c._password),
      _clients(c._clients), _bannedClients(c._bannedClients) {}


// 대입 연산자 오버로딩
Channel &Channel::operator=(const Channel &c)
{
    if (this != &c)
    {
        _isInviteOnly = c._isInviteOnly;
        _isTopicRestricted = c._isTopicRestricted;
        _channelName = c._channelName;
        _topic = c._topic;
        _password = c._password;
        _clients = c._clients;
        _operators = c._operators;
    }
    return *this;
}

bool    Channel::operator<( const Channel &c ) const
{
    return (this->_channelName < c._channelName);
}

bool	Channel::operator<=( const Channel &c ) const
{
	return (this->_channelName <= c._channelName);
}

bool	Channel::operator>( const Channel &c ) const
{
	return (this->_channelName > c._channelName);
}

bool	Channel::operator>=( const Channel &c ) const
{
	return (this->_channelName >= c._channelName);
}


bool Channel::isOwner(Client& client)
{
	if (this->_clientAuth[client.getNick()] == "OWNER")
		return true;
	return false;
}

bool Channel::isOperator(Client& client)
{
    return _operators.find(&client) != _operators.end();
}

bool Channel::checkmode(char mode)
{
    std::string modeStr(1, mode);

    if (this->_mode.find(modeStr) != this->_mode.end())
	{
		return true;
	}

	return false;
}

Channel::Channel(const std::string &ChannelName, Client &client, std::string key)
    : _isInviteOnly(false), _isTopicRestricted(false), _channelName(ChannelName)
{
    if (key != "") {
        this->_password = key;
    }
    else{
        this->_password = "";
    }
    this->_topic = "";
    _clients.insert(&client);
    this->_owner = &client;
    this->_clientAuth[client.getNick()] = "OWNER";
    this->_accessLimit = 1000000;
    this->addOperator(client);
    this->_create_time = time(NULL);
}

long long Channel::getChannelCreateTime(){
    return this->_create_time;
}


// 소멸자
Channel::~Channel() {}

// 클라이언트 추가
int Channel::addClient(Client &client)
{
    _clients.insert(&client);
    return 0;
}

// 오퍼레이터 추가
void Channel::addOperator(Client &client)
{
    _operators.insert(&client);
}

void Channel::dismissOperator(Client &client)
{
    _operators.erase(&client);
}

// 오퍼레이터 제거
int Channel::removeOperator(Client &client)
{
    _operators.erase(&client);
    return 0;
}

// 클라이언트 강퇴
int Channel::kickClient(Client &client)
{
    _clients.erase(&client);
    return 0;
}

std::string Channel::getWhoSetTopic(){
    return this->_topicSetUser;
}

std::string Channel::getTopicSetTime(){
    return this->_topicSetTime;
}

void Channel::addBan(Client& client)
{
	std::string name = client.getNick();

	this->_clientAuth.erase(name);
	this->_clients.erase(this->_clients.find(&client));
    this->_bannedClients.insert(&client);
}

bool Channel::checkBan(Client& client)
{
    if (this->_bannedClients.size() > 0)
    {
        std::set<Client*>::iterator it = this->_bannedClients.find(&client);
        if (it != this->_bannedClients.end()) {
            std::cout << "You are a banned client in this channel" << std::endl;
            return true;
        }
    }
    return false;
}

void Channel::setOwner(Client& client)
{
	this->_owner = &client;
}

void Channel::joinClient(Client& client, std::string auth)
{
	if (checkBan(client))
		return;
	if (auth == "OWNER")
		this->setOwner(client);
	std::string name = client.getNick();
	this->_clients.insert(&client);
    this->_clientAuth[client.getNick()] = auth;
}

// 토픽 설정
int Channel::setTopic(Client &client, const std::string &topic)
{
    char		buf[100];
	time_t		clock_timer;
    struct tm*	tm_struct;

	clock_timer = time(NULL);
	tm_struct = localtime(&clock_timer);
    
    _topicSetUser = client.getNick();
	strftime(buf, 100, "%Y-%m-%d %H:%M:%S", tm_struct);
	_topicSetTime = buf;
    _topic = topic;
    return (0);
}

unsigned int Channel::getUserCountLimit(){
    return this->_accessLimit;
}

void Channel::setPassword(std::string password){
    this->_password = password;
}

// 모드 설정
void Channel::setMode(std::string mode)
{
    this->_mode.insert(mode);
}

void Channel::delMode(std::string mode){
    this->_mode.erase(mode);
}

// 메시지 전송
int Channel::sendMessage(Client &client, const std::string &message)
{
    (void)client;
    (void)message;
    // 메시지 전송 로직 추가
    return 0;
}

void    Channel::setName( std::string name )
{
    this->_channelName = name;
}

// 채널에 속한 클라이언트들 반환
std::set<Client *> Channel::getClients()
{
    return _clients;
}

// 채널에 속한 클라이언트들 반환
Client* Channel::getClient(std::string nickname)
{
    for (std::set<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        Client* client = *it;
        if (client->getNick() == nickname)
        {
            return client;
        }
    }
    return NULL;
}


std::string Channel::getName(){
    return this->_channelName;
}

// 클라이언트 제거
int Channel::removeClient( Client &client )
{
    // 클라이언트 제거 로직을 추가
    _clients.erase(&client);

    // 클라이언트가 채널에 남아 있는 유일한 클라이언트였다면 채널 삭제
    if (_clients.empty())
    {
        // this->_server->removeChannel(this);
        // 채널을 삭제하는 로직 추가
    }

    return 0;
}

void Channel::setUserNumberLimit(unsigned int limit){
    this->_accessLimit = limit;
}

std::string Channel::getPassword(){
    return this->_password;
}

std::set<std::string> Channel::getMode(){
    return this->_mode;
}

bool Channel::getInviteMode() const {
	if (this->_mode.find("i") != this->_mode.end())
		return true;
	return false;
}

bool Channel::isMode(std::string mode){
    if (this->_mode.find(mode) != this->_mode.end())
        return true;
    return false;
}

bool Channel::checkInvite(std::string nickname)
{
    if (this->_inviteName.find(nickname) == this->_inviteName.end()){
        return false;
    }
    else{
        this->_inviteName.erase(nickname);
        return true;
    }
}

std::string Channel::getTopic(){
    return (this->_topic);
}

bool    Channel::isClient( std::string nick )
{
	ClientSet	clients = this->getClients();

	for (ClientSet::iterator it = clients.begin(); it != clients.end(); it++)
	{
		if (nick == (*it)->getNick())
            return (true);
	}

	return (false);
}

void	Channel::removeClient( std::string nick )
{
	ClientSet			clients = this->getClients();
	ClientSet::iterator	it = clients.begin();

	for (; it != clients.end(); it++)
	{
		// std::cout << "client " << (*it)->getNick() << std::endl;
		if (nick == (*it)->getNick())
		{
			std::cout << "remove client " << (*it)->getNick() << " from " << this->getName() << std::endl;
			clients.erase(it);  // segfault
            break;
		}
	}
	this->_clients = clients;
}

void	Channel::removeAuth( std::string nick )
{
	this->_clientAuth.erase(nick);
}

ClientSet	&Channel::getOperators()
{
    return (this->_operators);
}



 void   Channel::addInviteList(std::string name){
    this->_inviteName.insert(name);
 }