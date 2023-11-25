/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Define.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minjinki <minjinki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/18 11:03:11 by minjinki          #+#    #+#             */
/*   Updated: 2023/11/25 11:39:43 by minjinki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef DEFINE_HPP
#define DEFINE_HPP

#include <map>
#include <set>
#include <vector>

typedef std::map<int, Client *>				ClientMap;
typedef std::map<std::string, Channel *>	ChannelMap;
typedef std::vector<struct kevent>			EventVec;
typedef std::set<Channel *>					ChannelSet;
typedef std::set<Channel *>					InvitedSet;

#define SUCCESS 0
#define FAILURE -1

#define ERROR -1

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
#define RPL_WELCOME(user)													"001 " + user ":Welcome to our ircserv " + user + ". Enjoy your stay!"
#define RPL_YOURHOST(serverName)											"002 " + serverName + " :Your host is " + serverName + ", running version 1.0"

/*
 * Error Replies
 */
#define ERR_NOSUCHNICK(nick)												"401 :No such nick named " + nick
#define ERR_NOSUCHSERVER(server)											"402 :No such server named " + server
#define ERR_NOSUCHCHANNEL(channel)											"403 :No such channel named " + channel
#define ERR_CANNOTSENDTOCHAN(channel)										"404 :Cannot send to channel " + channel
#define ERR_TOOMANYCHANNELS()												"405 :You have joined too many channels"
// #define ERR_NOORIGIN()														"409 :missing the originator parameter"
#define ERR_NONICKNAMEGIVEN()												"431 :Nickname not given"
#define ERR_NICKNAMEINUSE(nick)												"433 " + nick + " :is already in use"
#define ERR_USERNOTINCHANNEL(nick, channel)									"441 " + nick + " " + channel + " :They are not on that channel"
#define ERR_NOTONCHANNEL(channel)											"442 " + channel + " :You are not in the channel"
#define ERR_USERONCHANNEL(user, nick, channel)								"443 " + nick + " " + channel + " :is already on channel"
#define ERR_NEEDMOREPARAMS(command)											"461 " + command + " :Need more parameters"
#define ERR_ALREADYREGISTRED()												"462 :You have already registered"
#define ERR_PASSWDMISMATCH()												"464 :Password incorrect"
#define ERR_YOUREBANNEDCREEP()												"465 :You are banned from this server"
#define ERR_CHANNELISFULL(channel)											"471 " + channel + " :Channel is full"
#define ERR_UNKNOWNMODE(mode)												"472 " + mode + " :unknown mode"
#define ERR_INVITEONLYCHAN(channel)											"473 " + channel + " :You are not invited"
#define ERR_BANNEDFROMCHAN(channel)											"474 " + channel + " :You are banned from this channel"
#define ERR_BADCHANNELKEY(user, channel)									"475 " + user + " " + channel + " :Cannot join channel (+k)"
#define ERR_CHANOPRIVSNEEDED(user, channel)									"482 " + user + " " + channel + " :You must be a channel operator"
#define ERR_CHANOPRIVSNEEDED2(user, channel)								"482 " + user + " " + channel + " :You must be a channel half-operator"
#define ERR_CHANOPRIVSNEEDEDMODE(user, channel, mode)						"482 " + user + " " + channel + " :You must have channel op access or above to set channel mode " + mode
#define ERR_NOOPPARAM(user, channel, mode, modename, param)					"696 " + user + " " + channel + " " + mode + " * :You must specify a parameter for the " + modename + " mode. Syntax: <" + param + ">." 
#define ERR_LONGPWD(user, channel)											":" + user + " " + channel + " :Too long password"
#define ERR_QUIT(user, message)												"ERROR :Closing link: (" + user + ") [Quit: " + message + "]"

/*
 * Command Responses
 */
#define RPL_ENDOFWHO(user, name)											"315 " + user + " " + name + " :End of /WHO list"
#define RPL_LISTSTART(user)													"321 " + user + " Channel :Users Name"
#define RPL_LIST(user, channel, visible, mode, topic)						"322 " + user + " " + channel + " " + visible + " :" + mode + " " + topic
#define RPL_LISTEND(user)														"323 " + user + ":End of /LIST"
#define RPL_CHANNELMODEIS(user, channel, modes, params)						"324 " + user + " " + channel + " " + modes + params
#define RPL_CHANNELCREATETIME(user, channel, date)							"329 " + user + " " + channel + " :" + date
#define RPL_NOTOPIC(user, channel)											"331 " + user + " " + channel + " :No topic is set"
#define RPL_TOPIC(user, channel, topic)										"332 " + user + " " + channel + " " + topic
#define RPL_TOPICWHOTIME(user, channel, nick, setat)						"333 " + user + " " + channel + " " + nick + " " + setat
#define RPL_INVITING(user, nick, channel)									"341 " + user + " " + nick + " :" + channel
#define RPL_WHOREPLY(client, channel, user, host, server, nick, opt, real)	"352 " + client + " " + channel + " " + user + " " + host + " " + server + " " + nick + " " + opt + " " + ":0 " + real
#define RPL_NAMREPLY(user, symbol, channel, users)							"353 " + user + " " + symbol + " " + channel + " :" + users
#define RPL_ENDOFNAMES(user, channel)										"366 " + user + " " + channel + " :End of /NAMES list."
#define RPL_ENDOFBANLIST(user, channel)										"368 " + user + " " + channel + " :End of channel ban list"

/*
 * Commands
 */
#define RPL_QUIT(user, message)												":" + user + " QUIT :Quit: " + message
#define RPL_PONG(user, ping)												":" + user + " PONG :" + ping
#define RPL_JOIN(user, channel)												":" + user + " JOIN :" + channel
#define RPL_PRIVMSG(user, target, msg)										":" + user + " PRIVMSG " + target + msg
#define RPL_MY_TOPIC(user, channel, topic)									":" + user + " TOPIC " + channel + " " + topic
#define RPL_PART(user, channel)												":" + user + " PART " + " :" + channel
#define RPL_KICK(user, channel, nick)										":" + user + " KICK " + channel + " " + nick + " :"
#define RPL_INVITE(user, nick, channel)										":" + user + " INVITE " + nick + " :" + channel
#define RPL_MODE(user, channel, modes, params)								":" + user + " MODE " + channel + " " + modes + params
#define RPL_NICK(before, after)												":" + before + " NICK :" + after

#endif