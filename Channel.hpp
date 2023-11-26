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

class Client;

class Channel
{
private:
    bool _isInviteOnly;
    bool _isTopicRestricted;

    std::string _channelName;
    std::string _topic;
    std::string _password;

    std::set<Client *> _clients;
    std::set<Client *> _operators;

    std::set<std::string> _mode;
    std::set<std::string> _inviteName;
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
    int removeClient(Client &client);
    int addOperator(Client &client);
    int removeOperator(Client &client);
    int kickClient(Client &client);
    int setTopic(Client &client, const std::string &topic);
    int setMode(Client &client, const std::string &mode);
    int sendMessage(Client &client, const std::string &message);
    
    std::string getPassword();
    bool getInviteMode() const;
    bool checkInvite(std::string nickname);
    
    // Public member functions to access private members
    void getAuth();
    std::set<Client *> getClients();
    std::string getName();
    std::map<std::string, Client> getUsers();
    std::set<std::string> getMode();


};

#endif