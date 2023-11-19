/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Define.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minjinki <minjinki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/18 11:03:11 by minjinki          #+#    #+#             */
/*   Updated: 2023/11/19 12:08:54 by minjinki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DEFINE_HPP
#define DEFINE_HPP

#include <map>
#include <vector>

#define SUCCESS 0
#define FAILURE -1

#define ERROR -1

#define RET_SUCCESS 0
#define RET_ERROR -1

#define FD_DEFAULT -1
#define FD_ERROR -1

#define MAX_CLIENT 30

typedef map<int, Client *>			ClientMap;
typedef map<std::string, Channal *>	ChannalMap;
typedef vector<struct kevent>		EventVec;
typedef vector<Chanal *>			ChannalVec;

#endif