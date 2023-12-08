/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cmd_TOPIC.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minjinki <minjinki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/02 15:32:26 by minjinki          #+#    #+#             */
/*   Updated: 2023/12/08 22:43:27 by minjinki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"

void	Command::topic( Server *server, Client *client, std::istringstream &iss )
{
	std::string		channelName;
	std::string		topic;

	iss >> channelName;
	std::getline(iss, topic);

	this->_removeCRLF(topic);

	if (channelName[0] == ':')	// 없애고 테스트 해보기
		channelName = channelName.substr(1);
	std::cout << "channelName: " << channelName << std::endl;
	if (topic[0] == ' ' || topic[0] == ':')
		topic = topic.substr(1);

	// 채널 이름 입력 x
	if (channelName.empty())
	{
		client->setSendData(ERR_NEEDMOREPARAMS(client->getNick(), std::string("TOPIC")) + CRLF);
		return ;
	}

	// 서버에 채널 없음
	Channel *channel = server->getChannel(channelName);
	if (!channel)
	{
		client->setSendData(ERR_NOSUCHCHANNEL(client->getNick(), channelName) + CRLF);
		return ;
	}

	// 사용자가 채널에 없음
	if (!channel->isClient(client->getNick()))
	{
		client->setSendData(ERR_NOTONCHANNEL(client->getNick(), channelName) + CRLF);
		return ;
	}

	// topic 가져오기
	if (topic.empty())
	{
		// topic 없음
		if (channel->getTopic().empty())
		{
			client->setSendData(RPL_NOTOPIC(client->getNick(), channelName) + CRLF);
		}
		// topic 있는 경우 토픽 && 설정한 사람, 시간
		else
		{
			std::cout << "getTopic(): " << channel->getTopic() << std::endl;
			std::cout << "getWhoSetTopic(): " << channel->getWhoSetTopic() << std::endl;
			std::cout << "getTopicSetTime(): " << channel->getTopicSetTime() << std::endl;
			client->setSendData(RPL_TOPIC(client->getNick(), channelName, channel->getTopic()) + CRLF);
			client->appendSendData(RPL_TOPICWHOTIME(client->getNick(), channelName, channel->getWhoSetTopic(), channel->getTopicSetTime()) + CRLF + CRLF);
		}

		return ;
	}

	// 변경 -> 권한 없음
	if (!channel->isOperator(*client) && channel->checkmode('t'))
	{
		client->setSendData(ERR_CHANOPRIVSNEEDED(channelName) + CRLF + CRLF);
		return ;
	}

	channel->setTopic(*client, topic);
	server->broadcast(channelName, RPL_TOPIC2(client->getNick(), channelName, topic) + CRLF);
}