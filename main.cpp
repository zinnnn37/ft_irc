/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minjinki <minjinki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/15 11:10:25 by minjinki          #+#    #+#             */
/*   Updated: 2023/11/15 13:09:42 by minjinki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void	leaks()
{
	system("leaks ircserv");
}

int	check_params(int ac, char **av)
{
	atexit(leaks);

	if (ac != 3)
	{
		std::cout << "Usage: ./ircserv <port> <password>" << std::endl;
		return (0);
	}
	else
	{
		char		*endptr;
		long		int	port = strtol(av[1], &endptr, 10);
		std::string	pass(av[2]);

		// 0 ~ 1023: well-known ports
		// 1024 ~ 49151: registered ports, 사용자가 소켓프로그래밍을 할 때 개인적으로 사용해도 되는 포트번호
		if (*endptr != '\0' || port < 1024 || port > 49151)
		{
			std::cout << "Invalid port" << std::endl;
			return (0);
		}

		if (pass.length() < 1)
		{
			std::cout << "Invalid password" << std::endl;
			return (0);
		}
	}
	return (1);
}

int	main(int ac, char **av)
{
	if (!check_params(ac, av))
		return (1);

	Server	server(atoi(av[1]), av[2]);

	try
	{
		server.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
}