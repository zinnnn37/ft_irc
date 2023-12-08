/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cmd_JOIN.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minjinki <minjinki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/05 09:14:30 by minjinki          #+#    #+#             */
/*   Updated: 2023/12/09 01:51:21 by minjinki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"
#include "Client.hpp"
#include "Define.hpp"

class Server;

// 채널 참가 처리 함수
std::string Command::_processJoinChannels(Server *server, Client &client, const std::string &channelName, const std::string &accessKey, std::string &result)
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
		Channel *channel = server->getChannel(*it);
		if (channel && channel->getClient(client.getNick()) != NULL)
		{
			result += ERR_USERONCHANNEL(client.getNick(), client.getNick(), *it) + CRLF;
			return result;
		}

        if (index < accessKeys.size()){
			std::string key = accessKeys[index];
            result += _clientJoinChannel(server, client, *it, accessKeys[index]);
		}
        else{
			std::string key = "";
            result += _clientJoinChannel(server, client, *it, key);

		}
        index++;
    }
	return result;
}

// JOIN 명령어 처리
void Command::join(Server *server, Client *client, std::istringstream &iss)
{
    // 클라이언트가 특정 채널에 참가하려는 경우를 처리하는 함수
    // 채널에 참가하는 동작을 수행하고, 결과를 반환
    // 결과는 문자열로 반환되어 클라이언트에게 전달

    std::string result; // 결과를 담을 문자열

    std::string channelName;
    std::string accessKey;

    iss >> channelName; // 버퍼에서 채널 이름 읽기
    iss >> accessKey;    // 버퍼에서 액세스 키 읽기

    // JOIN 명령에 필요한 매개변수가 충분하지 않을 경우 에러 메시지 생성
    if (channelName.empty())
        result += ERR_NEEDMOREPARAMS(client->getNick(), "JOIN") + CRLF;

    // 클라이언트가 최대 채널 수에 가입했을 경우 에러 메시지 생성
    else if (client->getChannels().size() > 10)
        result += ERR_TOOMANYCHANNELS(client->getNick()) + CRLF;


    // 채널과 액세스 키에 대한 처리를 담당하는 새로운 함수 호출
	else
	{
		// 채널 이름이 '#'로 시작하지 않으면 '#'을 추가
		if (channelName[0] != '#' && channelName[0] != '&')
			channelName = "#" + channelName;
		result = _processJoinChannels(server, *client, channelName, accessKey, result);
		std::cout << result << std::endl;
	}

	// 클라이언트에게 결과 전송
	client->setSendData(result + CRLF);
}

std::string makeCRLF2(std::string buffer)
{
	return buffer += "\r\n";
}

std::string Command::_clientJoinChannel(Server *server, Client &client, std::string &ch_name, std::string &key)
{
	std::string 	response;
	Channel 		*p_channel;

    if (server->isChannel(ch_name))
        p_channel = server->getChannel(ch_name);
    else
    {
		p_channel = server->createChannel(ch_name, key, client);
	}

	if (p_channel->getClients().size() + 1 > p_channel->getUserCountLimit())
	{
		// 채널의 제한 인원이 꽉 찼을 경우
		response += ERR_CHANNELISFULL(client.getNick(), p_channel->getName()) + CRLF;
		return response;
	}

	if ((!p_channel->getPassword().empty() && key.empty())  // 비밀 번호 존재하는데 client 쪽의 비밀번호가 없는 경우
		|| (!p_channel->getPassword().empty() && key != p_channel->getPassword()) // 비밀 번호가 다른 경우
		|| (p_channel->getPassword().empty() && !key.empty())) // 비밀 번호가 없는데 client가 비밀번호를 보낸 경우 
	{
		response += ERR_BADCHANNELKEY(client.getNick(), ch_name) + CRLF;
		return response;
	}

	if (p_channel->getInviteMode() && !p_channel->checkInvite(client.getNick()))
	{
		response += ERR_INVITEONLYCHAN(client.getNick(), ch_name) + CRLF;
		return response;
	}

	// 사용자 목록을 담을 빈 문자열 초기화
	// 채널의 사용자 목록을 얻어옴
	// server
	std::string s_users = "";
	std::set<Client *> users = p_channel->getClients();

	// 클라이언트가 사용자 목록에 없으면 채널에 가입
	if (users.find(&client) == users.end())
		p_channel->joinClient(client, "NORMAL");

	// 클라이언트를 채널에 가입시킴
	client.joinChannel(p_channel);

	// 채널의 모든 사용자에 대한 루프
	for (std::set<Client *>::iterator it = users.begin(); it != users.end(); ++it)
		s_users.append((*it)->getNick() + " "); // 사용자 목록에 사용자 이름을 추가
	
	// 채널 가입 메시지를 브로드캐스트
	server->broadcast(ch_name, RPL_JOIN(client.getPrefix(), ch_name) + CRLF);

	std::string who_in_channel = "channel : ";
    std::set<Client *> channel_in_user = p_channel->getClients();
	for (std::set<Client *>::iterator it = channel_in_user.begin(); it != channel_in_user.end(); ++it)
	{
		Client *currentClient = *it;
		who_in_channel +=  " " + currentClient->getNick();
	}
	std::cout << who_in_channel << std::endl;

	// 채널의 토픽이 비어있지 않으면 토픽과 관련된 응답을 추가
	if (!p_channel->getTopic().empty())
	{
		std::string msg1 = RPL_TOPIC2(client.getNick(), ch_name, p_channel->getTopic());
		std::string msg2 = RPL_TOPICWHOTIME(client.getUserName(), ch_name, p_channel->getWhoSetTopic(), p_channel->getTopicSetTime());
		response += makeCRLF2(msg1);
		response += makeCRLF2(msg2);
	}

	// 사용자 목록에 대한 응답을 추가
	std::string msg3 = RPL_NAMREPLY(client.getNick(), ch_name, s_users);
	response += makeCRLF2(msg3);

	// 사용자 목록의 끝에 대한 응답을 추가
	std::string msg4 = RPL_ENDOFNAMES(client.getNick(), ch_name);
	response += makeCRLF2(msg4);

	return response;
}


