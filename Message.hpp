/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minjinki <minjinki@student.42.kr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/21 13:09:20 by minjinki          #+#    #+#             */
/*   Updated: 2023/11/21 13:15:29 by minjinki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <string>
#include <vector>

class	Message
{
	private:
		std::string					_prefix;
		std::string					_command;
		std::vector<std::string>	_parameters;

		Message();
		Message( const Message &m );

		Message	&operator=( const Message &m );

		void	_parseMessage( std::string &message );

		std::vector<std::string>	_split( std::string &message, char delim );

	public:
		Message( std::string &message );
		~Message();

		const std::string				getCommand() const;
		const std::string				getPrefix() const;
		const std::vector<std::string>	getParameters() const;

		
};

#endif
