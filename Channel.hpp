/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minjinki <minjinki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/11 12:53:41 by minjinki          #+#    #+#             */
/*   Updated: 2023/11/26 17:09:24 by minjinki         ###   ########.fr       */
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


class Channel
{
private:
    bool _isInviteOnly;
    bool _isTopicRestricted;

    Client      *_owner;

    std::string _channelName;
    std::string _topic;
    std::string _password;
    std::string _topicSetUser;
    std::string _topicSetTime;

    std::set<Client *> _clients;
    std::set<Client *> _bannedClients;
    std::set<Client *> _operators;

    std::set<std::string> _mode;
    std::set<std::string> _inviteName;

    std::map<std::string, std::string> _clientAuth;

    // Private copy constructor and copy assignment operator

public:
    // Public default constructor and parameterized constructor
    Channel();
    Channel(const std::string &channelName, Client &client);
    Channel(const Channel &c);
    Channel &operator=(const Channel &c);

    bool    operator<( const Channel &c ) const;

    // Public destructor
    ~Channel();

    // Member functions
    int addClient(Client &client);
    int kickClient(Client &client);
    int addOperator(Client &client);
    int removeClient(Client &client);
    int removeOperator(Client &client);
    int setMode(Client &client, const std::string &mode);
    int setTopic(Client &client, const std::string &topic);
    int sendMessage(Client &client, const std::string &message);
    
    bool getInviteMode() const;
    bool checkBan(Client& client);
    bool checkInvite(std::string nickname);

    // Public member functions to access private members
    void getAuth();
    void addBan(Client& client);
    void joinClient(Client& client, std::string auth);
    void setOwner(Client& client);

    std::string getName();
    std::set<std::string> getMode();
    std::set<Client *> getClients();
    std::map<std::string, Client> getUsers();

    std::string getTopic();
    std::string getPassword();
    std::string getWhoSetTopic();
    std::string getTopicSetTime();
};

#endif