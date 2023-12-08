/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minjinki <minjinki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/11 12:53:41 by minjinki          #+#    #+#             */
/*   Updated: 2023/12/09 01:37:50 by minjinki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <set>
#include <map>

#include "Client.hpp"
#include "Define.hpp"
#include "Server.hpp"

class Server;

class Channel
{
private:
    unsigned int _accessLimit;

    bool _isInviteOnly;
    bool _isTopicRestricted;

    std::string _create_time;
    Client      *_owner;
    std::string _channelName;
    std::string _topic;
    std::string _password;
    std::string _topicSetUser;
    std::string _topicSetTime;

    std::set<Client *> _clients;
    //std::set<Client *> _bannedClients;
    std::set<Client *> _operators;

    std::set<std::string> _mode;
    std::set<std::string> _inviteName;

    std::map<std::string, std::string> _clientAuth;

    // Private copy constructor and copy assignment operator

public:
    // Public default constructor and parameterized constructor
    Channel();
    Channel(const std::string &channelName, Client &client, std::string key);
    Channel(const Channel &c);
    Channel &operator=(const Channel &c);

    bool    operator<( const Channel &c ) const;
	bool	operator<=( const Channel &c ) const;
	bool	operator>( const Channel &c ) const;
	bool	operator>=( const Channel &c ) const;

    // Public destructor
    ~Channel();

    // Member functions
    int addClient(Client &client);
    int kickClient(Client &client);
    int removeClientinServer(Server *server, Client &client);
    int removeOperator(Client &client);
    int setTopic(Client &client, const std::string &topic);
    unsigned int getUserCountLimit();
    
    bool getInviteMode() const;
    //bool checkBan(Client& client);
    bool checkInvite(std::string nickname);
    bool isOwner(Client& client);
    bool isOperator(Client& client);
    bool checkmode(char mode);
    bool isMode(std::string mode);

    std::string getAuth(const std::string &nick );
    //void addBan(Client& client);
    void joinClient(Client& client, std::string auth);
    void setOwner(Client& client);
    void setMode(std::string mode);
    void setPassword(std::string password);
    void setUserNumberLimit(unsigned int limit);
    void delMode(std::string mode);
    void addOperator(Client &client);
    void dismissOperator(Client &client);
    void addInviteList(std::string name);
    std::string getChannelCreateTime();
    void setName( std::string name );

    Client *getClient(std::string nickname);

    std::string getName();
    std::set<std::string> getMode();
    std::set<Client *> getClients();
    // std::map<std::string, Client> getUsers();

    std::string getTopic();
    std::string getPassword();
    std::string getWhoSetTopic();
    std::string getTopicSetTime();

    bool    isClient( std::string nick );

    void    removeClient(std::string nick );
	void	removeAuth( std::string nick );

    ClientSet   &getOperators();
};

#endif