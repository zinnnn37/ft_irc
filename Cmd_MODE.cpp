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

    if ((channel_name[0] != '#') || (channel_mode.empty())
        || (server->getChannels().find(channel_name) == server->getChannels().end()))
    {
        std::cout << "ERROR: Wrong value is typed\n";
        return ;
    }

    Channel *ch = server->getChannel(channel_name);


// i (Invite-only channel):
// +i: Invite-only 채널로 설정 / 1
// -i: Invite-only 채널 설정 제거 / -1

// t (TOPIC command restrictions to channel operators):
// +t: TOPIC 명령에 대한 제한을 채널 운영자로 설정
// -t: TOPIC 명령에 대한 제한 설정 제거

// k (Channel key - password):
// +k: 채널 키(비밀번호) 설정
// -k: 채널 키(비밀번호) 설정 제거

// o (Channel operator privilege):
// +o: 사용자에게 채널 운영자 권한 부여
// -o: 사용자로부터 채널 운영자 권한 박탈

// l (User limit to channel):
// +l: 채널에 사용자 제한 설정
// -l: 채널에 사용자 제한 설정 제거
            // 소유자가 아님 운영진이 아님 -> false
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
                if (ch->checkmode(channel_mode[i])) continue; // 이미 해당 조건이 있다면 넘어감
                std::string tmp_mode(1, channel_mode[i]);
                ch->setMode(tmp_mode);
                mode_msg += channel_mode[i];
                pre_plus_minu = 1;
            }
            else {
                if (!ch->checkmode(channel_mode[i])) continue; // 해당 조건이 없다면 넘어감
                std::string tmp_mode(1, channel_mode[i]);
                ch->delMode(tmp_mode);
                mode_msg += channel_mode[i];
                pre_plus_minu = -1;
            }
            
        }
        // t (TOPIC command restrictions to channel operators):
        // +t: TOPIC 명령에 대한 제한을 채널 운영자로 설정
        // -t: TOPIC 명령에 대한 제한 설정 제거
        else if (channel_mode[i] == 't'){
            if (plus_minus == 1){
                if (ch->checkmode(channel_mode[i])) continue; // 이미 해당 조건이 있다면 넘어감
                std::string tmp_mode(1, channel_mode[i]);
                ch->setMode(tmp_mode);
                mode_msg += channel_mode[i];
                pre_plus_minu = 1;
            }
            else {
                if (!ch->checkmode(channel_mode[i])) continue; // 해당 조건이 없다면 넘어감
                std::string tmp_mode(1, channel_mode[i]);
                ch->delMode(tmp_mode);
                mode_msg += channel_mode[i];
                pre_plus_minu = -1;
            }
            
        }
        // k (Channel key - password):
        // +k: 채널 키(비밀번호) 설정
        // -k: 채널 키(비밀번호) 설정 제거
        else if (channel_mode[i] == 'k'){
            std::string new_password;
            iss >> new_password;

            if (new_password.empty()){
                std::cout << "ERROR: k option must be have password\n";
            }
            if (new_password.length() > 20){
                std::cout << "ERROR: password length is limited under 20\n";
            }

            if (plus_minus == 1){
                if (ch->checkmode(channel_mode[i])) continue; // 이미 해당 조건이 있다면 넘어감
                std::string tmp_mode(1, channel_mode[i]);
                ch->setPassword(new_password);
                ch->setMode(tmp_mode);
                mode_msg += channel_mode[i];
                pre_plus_minu = 1;
            }
            else {
                if (!ch->checkmode(channel_mode[i])) continue; // 해당 조건이 없다면 넘어감
                std::string tmp_mode(1, channel_mode[i]);
                new_password = "";
                ch->setPassword(new_password);
                ch->delMode(tmp_mode);
                mode_msg += channel_mode[i];
                pre_plus_minu = -1;
            }
        }
        // o (Channel operator privilege):
        // +o: 사용자에게 채널 운영자 권한 부여
        // -o: 사용자로부터 채널 운영자 권한 박탈
        else if (channel_mode[i] == 'o') {
            std::string new_operator_name;
            iss >> new_operator_name;

            if (new_operator_name.empty()) {
                std::cout << "ERROR: o option must have an owner name\n";
                continue;  // 처리를 중단하고 다음 반복으로 이동
            }

            Client* new_operator = ch->getClient(new_operator_name);

            if (new_operator == nullptr) {
                std::cout << "ERROR: o option's client must be in this channel\n";
                continue;  // 처리를 중단하고 다음 반복으로 이동
            }

            if (plus_minus == 1) {
                if (ch->isOperator(*new_operator))
                    continue;  // 이미 소유자거나 운영자인 경우
                ch->addOperator(*new_operator);
                mode_msg += channel_mode[i];
                pre_plus_minu = 1;
            } else {
                if (!ch->isOperator(*new_operator))
                    continue;  // 소유자이거나 운영자가 아닌 경우
                ch->dismissOperator(*new_operator);
                mode_msg += channel_mode[i];
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
                if (limit.empty())
                    std::cout << "ERROR: l option must  must be followed by the number\n";
                if (limit.length() > 10)
                    limit = "0";

                unsigned int LimitNumber = atol(limit.c_str());
                if (LimitNumber < ch->getClients().size()){
                    std::cout << "ERROR: mode l limit user number is lower than current channel user\n";
                    continue;
                }
                if (ch->checkmode(channel_mode[i]) && LimitNumber == ch->getUserCountLimit()){
                    std::cout << "ERROR: mode l count is same before user limit\n";
                    continue;
                }
                ch->setUserNumberLimit(LimitNumber);
                std::string tmp(1, channel_mode[i]);
                ch->setMode(tmp);
                mode_msg += channel_mode[i];
                pre_plus_minu = 1;
            }
            else {
                if (!ch->checkmode(channel_mode[i])){
                    std::cout << "ERROR: mode l is not assgined\n";
                    continue;
                }
                ch->setUserNumberLimit(10000000);
                std::string tmp(1, channel_mode[i]);
                ch->delMode(tmp);
                mode_msg += channel_mode[i];
                pre_plus_minu = 1;
            }
        }

        if (mode_msg.empty())
            mode_msg.insert(0, ":");
        if (!mode_msg.empty()){
            std::string msg = ":" + client->getNick() + " MODE " + channel_name + " " + mode_msg;
            server->broadcast(channel_name, msg);
        } 
    }
}