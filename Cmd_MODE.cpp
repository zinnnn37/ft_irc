/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cmd_MODE.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minjinki <minjinki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/05 09:14:40 by minjinki          #+#    #+#             */
/*   Updated: 2023/12/08 22:26:02 by minjinki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include "Server.hpp"
#include <algorithm>

void Command::mode(Server *server, Client *client, std::istringstream &iss){
    std::string channel_name;
    std::string channel_mode;
    iss >> channel_name;
    iss >> channel_mode;

	if (((channel_name[0] != '#') && (channel_name[0] != '&')) || (server->getChannels().find(channel_name) == server->getChannels().end()))
    {
        std::cout << "ERROR: Wrong value is typed\n";
        return ;
    }

    Channel *ch = server->getChannel(channel_name);

    if (channel_mode.empty()) {
        std::string reply;
        std::string response;
        std::string channel_prefix = "+";
        std::string channel_parameters = "";
        std::string channel_name = ch->getName();

        int key_mode_count = 0;
        int key_mode_added = 0;

        if (ch->isMode("k"))
            key_mode_added++;
        if (ch->isMode("l"))
            key_mode_added++;
        
        std::set<std::string> channel_modes = ch->getMode();

        if (key_mode_added == 0)
            channel_prefix = ":+";
        for (std::set<std::string>::iterator mode_it = channel_modes.begin(); mode_it != channel_modes.end(); mode_it++) {
            if (*mode_it == "k") {
                if (key_mode_count == key_mode_added - 1)
                    channel_parameters += " :";
                else
                    channel_parameters += " ";
                channel_parameters += ch->getPassword();
                key_mode_count++;
            }
            else if (*mode_it == "l") {
                if (key_mode_count == key_mode_added - 1)
                    channel_parameters += " :";
                else
                    channel_parameters += " ";
                channel_parameters += ch->getUserCountLimit();
                key_mode_count++;
            }
            channel_prefix += *mode_it;
        }

        response = RPL_CHANNELMODEIS(client->getNick(), channel_name, channel_prefix, channel_parameters) + CRLF;
        response += RPL_CHANNELCREATETIME(client->getNick(), channel_name, ch->getChannelCreateTime()) + CRLF;

        server->broadcast(channel_name, response);
        client->setSendData(response);
        return;
    }


    int plus_minus = 1;
    int pre_plus_minu = 0;
    std::string mode_msg = "";
    if ((ch->isOwner(*client) || ch->isOperator(*client))){
        plus_minus = 1;
    }
    else {
        std::cout <<  "ERROR: client is not this Channel OWNER or OPERATOR\n";
        client->setSendData(ERR_CHANOPRIVSNEEDED(channel_name) + CRLF);
        return ;
    }
    for (unsigned long i = 0; i < channel_mode.length(); i++){
        if (channel_mode[i] == '+' || channel_mode[i] == '-'){
            if (channel_mode[i + 1] == '+' || channel_mode[i + 1] == '-' 
                || channel_mode[i + 1] == ' '){
                std::cout <<  "ERROR: The +- operator must be followed by the mode.\n";
                client->setSendData("ERROR: The +- operator must be followed by the mode.\r\n");
                return ;
            }
            if (channel_mode[i] == '+') plus_minus = 1;
            else plus_minus = -1;
            mode_msg += channel_mode[i];
        }
        // i (Invite-only channel):
        // +i: Invite-only 채널로 설정 / 1
        // -i: Invite-only 채널 설정 제거 / -1
        else if (channel_mode[i] == 'i'){
            if (plus_minus == 1){
                mode_msg += channel_mode[i];
                if (ch->checkmode(channel_mode[i]))
                    continue; // 이미 해당 조건이 있다면 넘어감
                std::string tmp_mode(1, channel_mode[i]);
                ch->setMode(tmp_mode);
                pre_plus_minu = 1;
            }
            else {
                mode_msg += channel_mode[i];
                if (!ch->checkmode(channel_mode[i])) continue; // 해당 조건이 없다면 넘어감
                std::string tmp_mode(1, channel_mode[i]);
                ch->delMode(tmp_mode);
                pre_plus_minu = -1;
            }
            
        }
        // t (TOPIC command restrictions to channel operators):
        // +t: TOPIC 명령에 대한 제한을 채널 운영자로 설정
        // -t: TOPIC 명령에 대한 제한 설정 제거
        else if (channel_mode[i] == 't'){
            if (plus_minus == 1){
                mode_msg += channel_mode[i];
                if (ch->checkmode(channel_mode[i])) continue; // 이미 해당 조건이 있다면 넘어감
                std::string tmp_mode(1, channel_mode[i]);
                ch->setMode(tmp_mode);
                pre_plus_minu = 1;
            }
            else {
                mode_msg += channel_mode[i];
                if (!ch->checkmode(channel_mode[i])) continue; // 해당 조건이 없다면 넘어감
                std::string tmp_mode(1, channel_mode[i]);
                ch->delMode(tmp_mode);
                pre_plus_minu = -1;
            }
            
        }
        // k (Channel key - password):
        // +k: 채널 키(비밀번호) 설정
        // -k: 채널 키(비밀번호) 설정 제거
        else if (channel_mode[i] == 'k'){
            std::string new_password;
            iss >> new_password;

            std::cout << "new_password: " << new_password << "\n";

            if (plus_minus == 1){
                if (new_password.empty()){
                    std::cout << "ERROR: k option must be have password\n";
                    client->setSendData(ERR_NEEDMOREPARAMS(client->getNick(), std::string("MODE +k")) + CRLF);
                    return ;
                }
                if (new_password.length() > 20){
                    std::cout << "ERROR: password length is limited under 20\n";
                    client->setSendData("ERROR: password length is limited under 20\r\n");
                    return ;
                }
                mode_msg += channel_mode[i];
                std::string tmp_mode(1, channel_mode[i]);
                ch->setPassword(new_password);
                ch->setMode(tmp_mode);
                pre_plus_minu = 1;
            }
            else {
                mode_msg += channel_mode[i];
                if (!ch->checkmode(channel_mode[i])) continue; // 해당 조건이 없다면 넘어감
                std::string tmp_mode(1, channel_mode[i]);
                new_password = "";
                ch->setPassword(new_password);
                ch->delMode(tmp_mode);
                pre_plus_minu = -1;
            }
        }
        // o (Channel operator privilege):
        // +o: 사용자에게 채널 운영자 권한 부여
        // -o: 사용자로부터 채널 운영자 권한 박탈
        else if (channel_mode[i] == 'o') {
            std::string new_operator_name;
            iss >> new_operator_name;

            std::cout << "new_operator_name: " << new_operator_name << "\n";

            if (new_operator_name.empty()) {
                std::cout << "ERROR: o option must have an owner name\n";
                client->setSendData(ERR_NEEDMOREPARAMS(client->getNick(), std::string("MODE +o")) + CRLF);
                continue;  // 처리를 중단하고 다음 반복으로 이동
            }

            Client* new_operator = ch->getClient(new_operator_name);

            if (new_operator == NULL) {
                std::cout << "ERROR: o option's client must be in this channel\n";
                client->setSendData(ERR_USERNOTINCHANNEL(client->getNick(), new_operator_name, channel_name) + CRLF);
                continue;  // 처리를 중단하고 다음 반복으로 이동
            }

            if (plus_minus == 1) {
                mode_msg += channel_mode[i];
                if (ch->isOperator(*new_operator))
                    continue;  // 이미 소유자거나 운영자인 경우
                ch->addOperator(*new_operator);
                pre_plus_minu = 1;
            } else {
                mode_msg += channel_mode[i];
                if (!ch->isOperator(*new_operator))
                    continue;  // 소유자이거나 운영자가 아닌 경우
                ch->dismissOperator(*new_operator);
                pre_plus_minu = -1;
            }
        }


        // l (User limit to channel):
        // +l: 채널에 사용자 제한 설정
        // -l: 채널에 사용자 제한 설정 제거
        else if (channel_mode[i] == 'l'){
            std::string limit;
            iss >> limit;
            if (plus_minus == 1){
                std::cout <<  "mode l channel name: " <<  ch->getName() << "\n";
                std::cout <<  "mode l channel name: " <<  channel_name  << "\n";
                if (limit.empty())
                {
                    std::cout << "ERROR: l option must  must be followed by the number\n";
                    client->setSendData(ERR_NEEDMOREPARAMS(client->getNick(), std::string("MODE +l")) + CRLF);
                    continue ;
                }
                if (limit.length() > 10)
                    limit = "0";

                unsigned int LimitNumber = atol(limit.c_str());
                if (LimitNumber < ch->getClients().size()){
                    std::cout << "ERROR: mode l limit user number is lower than current channel user\n";
                    client->setSendData("ERROR: mode l limit user number is lower than current channel user\r\n");
                    continue;
                }
                if (ch->checkmode(channel_mode[i]) && LimitNumber < ch->getUserCountLimit()){
                    std::cout << "ERROR: mode l count is same before user limit\n";
                    client->setSendData("ERROR: mode l count is same before user limit\r\n");
                    continue;
                }
                mode_msg += channel_mode[i];
                ch->setUserNumberLimit(LimitNumber);
                std::string tmp(1, channel_mode[i]);
                ch->setMode(tmp);
                pre_plus_minu = 1;
            }
            else {
                if (!ch->checkmode(channel_mode[i])){
                    std::cout << "ERROR: mode l is not assgined\n";
                    client->setSendData("ERROR: mode l is not assgined\r\n");
                    continue;
                }
                mode_msg += channel_mode[i];
                ch->setUserNumberLimit(10000000);
                std::string tmp(1, channel_mode[i]);
                ch->delMode(tmp);
                pre_plus_minu = 1;
            }
        }

    }
    if (mode_msg.empty())
        mode_msg.insert(0, ":");
    std::cout << "mode_msg: " << mode_msg << "\n";
    std::cout << "length: " << mode_msg.length() << "\n";
    if (!mode_msg.empty() && (mode_msg[0] != '+' || mode_msg[0] != '-') && mode_msg.length() != 1){
        std::string msg = ":" + client->getNick() + " MODE " + channel_name + " " + mode_msg;
        server->broadcast(channel_name, msg);
    } 
}