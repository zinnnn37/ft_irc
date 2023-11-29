/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Define.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minjinki <minjinki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/18 11:03:11 by minjinki          #+#    #+#             */
/*   Updated: 2023/11/29 15:36:15 by minjinki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef DEFINE_HPP
#define DEFINE_HPP

#include <map>
#include <set>
#include <vector>

class Client;
class Channel;

typedef std::map<int, Client *>				ClientMap;
typedef std::map<std::string, Channel *>	ChannelMap;
typedef std::vector<struct kevent>			EventVec;
typedef std::set<Channel *>					ChannelSet;
typedef std::set<Channel *>					InvitedSet;
typedef std::set<Client *>					ClientSet;

#define SUCCESS 0
#define FAILURE -1

#define ERROR -1
#define CHANNEL 1
#define NICKNAME 2

#define RET_SUCCESS 0
#define RET_ERROR -1

#define FD_DEFAULT -1
#define FD_ERROR -1

#define MAX_CLIENT 30

#define CRLF "\r\n"

/*
 * colors
 */
#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define YELLOW "\033[0;33m"
#define BLUE "\033[0;34m"
#define PURPLE "\033[0;35m"
#define CYAN "\033[0;36m"
#define RESET "\033[0m"

/*
 * Server messages
 */
#define RPL_WELCOME(nick)											"001 " + nick + ":Welcome to our ircserv " + nick + ". Enjoy your stay!"
#define RPL_YOURHOST(serverName)									"002 " + serverName + " :Your host is " + serverName + ", running version 1.0"
// -> Server.hpp에 hostname 변수 추가

/*
 * Error Replies
 */
#define ERR_NOSUCHNICK(nick)										"401 :No such nick named " + nick
#define ERR_NOSUCHSERVER(server)									"402 :No such server named " + server
#define ERR_NOSUCHCHANNEL(channel)									"403 :No such channel named " + channel
#define ERR_CANNOTSENDTOCHAN(channel)								"404 :Cannot send to channel " + channel
#define ERR_TOOMANYCHANNELS()										"405 :You have joined too many channels"
#define ERR_NONICKNAMEGIVEN()										"431 :Nickname not given"
#define ERR_NICKNAMEINUSE(nick)										"433 " + nick + " :is already in use"
#define ERR_USERNOTINCHANNEL(nick, channel)							"441 " + nick + " " + channel + " :They are not on that channel"
#define ERR_NOTONCHANNEL(channel)									"442 " + channel + " :You are not in the channel"
#define ERR_USERONCHANNEL(nick, channel)							"443 " + nick + " " + channel + " :is already on channel"
#define ERR_NOTREGISTERED()											"451 :You have not registered"
#define ERR_NEEDMOREPARAMS(command)									"461 " + command + " :Need more parameters"
#define ERR_ALREADYREGISTRED()										"462 :You have already registered"
#define ERR_PASSWDMISMATCH()										"464 :Password incorrect "
#define ERR_YOUREBANNEDCREEP()										"465 :You are banned from this server"
#define ERR_CHANNELISFULL(channel)									"471 " + channel + " :Channel is full"
#define ERR_UNKNOWNMODE(mode)										"472 " + mode + " :unknown mode"
#define ERR_INVITEONLYCHAN(channel)									"473 " + channel + " :You are not invited"
#define ERR_BANNEDFROMCHAN(channel)									"474 " + channel + " :You are banned from this channel"
#define ERR_BADCHANNELKEY(channel)									"475 " + channel + " :Wrong channel key"
#define ERR_CHANOPRIVSNEEDED(channel)								"482 " + channel + " :You are not channel operator"
// #define ERR_NOOPPARAM(nick, channel, mode, modename, param)			"696 " + nick + " " + channel + " " + mode + " * :You must specify a parameter for the " + modename + " mode. Syntax: <" + param + ">." 
// #define ERR_LONGPWD(nick, channel)									":" + nick + " " + channel + " :Too long password"
// #define ERR_QUIT(nick, message)										"ERROR :Closing link: (" + nick + ") [Quit: " + message + "]"

/*
 * Command Responses
 */
#define RPL_ENDOFWHO(name)											"315 " + name + " :End of /WHO list"
#define RPL_LISTSTART()												"321 Channels :Users Name"
#define RPL_LIST(channel, visible, topic)							"322 " + channel + " " + visible + " :" + topic
#define RPL_LISTEND()												"323 :End of /LIST"
#define RPL_CHANNELMODEIS(channel, mode, mode_params)				"324 " + channel + " " + mode + " " + mode_params
#define RPL_CHANNELCREATETIME(channel, date)						"329 " + channel + " :created at " + date
#define RPL_NOTOPIC(channel)										"331 " + channel + " :Topic not set"
#define RPL_TOPIC(channel, topic)									"332 " + channel + " :" + topic
#define RPL_TOPICWHOTIME(channel, nick, setat)						"333 " + channel + " " + nick + " " + setat
// topic이 마지막으로 변경된 시간과 변경한 사용자
#define RPL_INVITING(nick, channel)									"341 " + nick + " " + channel
#define RPL_WHOREPLY(channel, user, host, server, nick, opt, real)	"352 " + channel + " " + user + " " + host + " " + server + " " + nick + " " + opt + " " + ":0 " + real
// :0은 hopcount, 서버와 사용자 사이의 거리
#define RPL_NAMREPLY(nick, channel, users)							"353 " + nick + " " + channel + " :" + users
#define RPL_ENDOFNAMES(channel)										"366 " + channel + " :End of /NAMES list."
#define RPL_ENDOFBANLIST(channel)									"368 " + channel + " :End of Ban list"

/*
 * Commands
 */
#define RPL_QUIT(nick, message)										":" + nick + " QUIT :Quit: " + message
#define RPL_PONG(nick, ping)										":" + nick + " PONG :" + ping
#define RPL_JOIN(nick, channel)										":" + nick + " JOIN :" + channel
#define RPL_PRIVMSG(nick, target, msg)								":" + nick + " PRIVMSG " + target + msg
#define RPL_MY_TOPIC(nick, channel, topic)							":" + nick + " TOPIC " + channel + " " + topic
#define RPL_PART(nick, channel)										":" + nick + " PART " + " :" + channel
#define RPL_KICK(nick, channel, nick2)								":" + nick + " KICK " + channel + " " + nick2 + " :"
#define RPL_INVITE(nick, nick2, channel)							":" + nick + " INVITE " + nick2 + " :" + channel
#define RPL_MODE(nick, channel, modes, params)						":" + nick + " MODE " + channel + " " + modes + params
#define RPL_NICK(before, after)										":" + before + " NICK :" + after

#endif