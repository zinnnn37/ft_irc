#include "Channel.hpp"

// 기본 생성자
Channel::Channel() : _isInviteOnly(false), _isTopicRestricted(false) {}

// 복사 생성자
Channel::Channel(const Channel &c)
    : _isInviteOnly(c._isInviteOnly), _isTopicRestricted(c._isTopicRestricted),
      _channelName(c._channelName), _topic(c._topic), _password(c._password),
      _clients(c._clients), _operators(c._operators) {}


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

// 생성자
Channel::Channel(const std::string &ChannelName, Client &client)
    : _isInviteOnly(false), _isTopicRestricted(false), _channelName(ChannelName)
{
    _clients.insert(&client);
}

// 소멸자
Channel::~Channel() {}

// 클라이언트 추가
int Channel::addClient(Client &client)
{
    _clients.insert(&client);
    return 0; // 성공을 나타내는 값, 실패 시 적절한 값으로 변경
}

// 오퍼레이터 추가
int Channel::addOperator(Client &client)
{
    _operators.insert(&client);
    return 0; // 성공을 나타내는 값, 실패 시 적절한 값으로 변경
}

// 오퍼레이터 제거
int Channel::removeOperator(Client &client)
{
    _operators.erase(&client);
    return 0; // 성공을 나타내는 값, 실패 시 적절한 값으로 변경
}

// 클라이언트 강퇴
int Channel::kickClient(Client &client)
{
    _clients.erase(&client);
    return 0; // 성공을 나타내는 값, 실패 시 적절한 값으로 변경
}

// 토픽 설정
int Channel::setTopic(Client &client, const std::string &topic)
{
    (void)client;
    _topic = topic;
    return 0; // 성공을 나타내는 값, 실패 시 적절한 값으로 변경
}

// 모드 설정
int Channel::setMode(Client &client, const std::string &mode)
{
    (void)client;
    (void)mode;
    // 모드 설정 로직 추가
    return 0; // 성공을 나타내는 값, 실패 시 적절한 값으로 변경
}

// 메시지 전송
int Channel::sendMessage(Client &client, const std::string &message)
{
    (void)client;
    (void)message;
    // 메시지 전송 로직 추가
    return 0; // 성공을 나타내는 값, 실패 시 적절한 값으로 변경
}

// 채널에 속한 클라이언트들 반환
std::set<Client *> Channel::getClients()
{
    return _clients;
}

std::string Channel::getName(){
    return this->_channelName;
}

// 클라이언트 제거
int Channel::removeClient(Client &client)
{
    // 클라이언트 제거 로직을 추가
    _clients.erase(&client);

    // 클라이언트가 채널에 남아 있는 유일한 클라이언트였다면 채널 삭제
    if (_clients.empty())
    {
        // this->_server->removeChannel(this);
        // 채널을 삭제하는 로직 추가
    }

    return 0; // 성공을 나타내는 값, 실패 시 적절한 값으로 변경
}


// std::map<std::string, Client> Channel::getUsers(){
//     // return this->getUsers;
//     return ;
// }