#include "Command.hpp"











std::string Server::handleMode(Client &client, std::stringstream &buffer_stream)
{
	std::string response;
	std::string ch_name;
	std::string modes;

	buffer_stream >> ch_name;
	buffer_stream >> modes;

	// 채널명 잘못 들어온 경우(유저 찾는 경우) 401 에러
	if (ch_name[0] != '#')
		return response;

	// 존재하지 않는 채널인 경우 403 에러
	if (channels.find(ch_name) == channels.end())
	{
		response = ERR_NOSUCHCHANNEL(client.getNickname(), ch_name);
		return response;
	}

	Channel* p_channel = channels[ch_name];

	// 채널명만 들어온 경우 해당 채널의 모드 반환
	if (modes.empty())
	{
		response = getChannelModeResponse(client, p_channel);
		return response;
	}
	if (modes == "b")
	{
		response = RPL_ENDOFBANLIST(client.getNickname(), ch_name);
		return response;
	}

	int flag = 1;
	int pre_flag = 0;
	std::string ch_modes = "";
	std::string ch_params = "";

	// i t k l o
	for (int i = 0; i < modes.length(); i++)
	{
		if (modes[i] == '+')
			flag = 1;
		else if (modes[i] == '-')
			flag = -1;
		else if (modes[i] == 'i' || modes[i] == 't' || modes[i] == 'n')
		{
			//op error
			if (!p_channel->isOperator(client))
				response += makeCRLF(ERR_CHANOPRIVSNEEDEDMODE(client.getNickname(), ch_name, modes[i]));
			else if (flag == 1)
			{
				if (p_channel->findMode(modes[i]))
					continue;
				
				p_channel->addMode(modes[i]);

				if (pre_flag == -1 || pre_flag == 0)
					ch_modes += "+";
				ch_modes += modes[i];
				pre_flag = 1;
			}
			else if (flag == -1)
			{
				if (!p_channel->findMode(modes[i]))
					continue;
				
				p_channel->eraseMode(modes[i]);

				if (pre_flag == 1 || pre_flag == 0)
					ch_modes += "-";
				ch_modes += modes[i];
				pre_flag = -1;
			}
		}
		else if (modes[i] == 'k')
		{
			std::string param;
			buffer_stream >> param;

			//param error
			if (param.empty())
				response += makeCRLF(ERR_NOOPPARAM(client.getNickname(), ch_name, modes[i], "key", "key"));
			// op error
			else if (!p_channel->isOperator(client))
				response += makeCRLF(ERR_CHANOPRIVSNEEDEDMODE(client.getNickname(), ch_name, modes[i]));
			// pwd error
			else if (param.length() > 20)
				response += makeCRLF(ERR_LONGPWD(client.getNickname(), ch_name));
			// +k
			else if (flag == 1)
			{
				if (p_channel->findMode(modes[i]))
					continue;
				
				p_channel->setPassword(param);
				p_channel->addMode(modes[i]);
				if (pre_flag == -1 || pre_flag == 0)
					ch_modes += "+";
				ch_modes += modes[i];
				ch_params += " " + param;
				pre_flag = 1;
			}
			// -k
			else if (flag == -1)
			{
				if (!p_channel->findMode(modes[i]))
					continue;
				if (param != p_channel->getPassword())
					continue;
				std::string pw = "";
				p_channel->setPassword(pw);
				p_channel->eraseMode(modes[i]);
				if (pre_flag == 1 || pre_flag == 0)
					ch_modes += "-";
				ch_modes += modes[i];
				ch_params += " " + param;
				pre_flag = -1;
			}
		}
		else if (modes[i] == 'l')
		{
			std::string param;
			buffer_stream >> param;

			// op error
			if (!p_channel->isOperator(client))
				response += makeCRLF(ERR_CHANOPRIVSNEEDEDMODE(client.getNickname(), ch_name, modes[i]));
			// -l
			else if (flag == -1)
			{
				if (!p_channel->findMode(modes[i]))
					continue;

				p_channel->setUserLimit(999999999);
				p_channel->eraseMode(modes[i]);
				if (pre_flag == 1 || pre_flag == 0)
					ch_modes += "-";
				ch_modes += modes[i];
				pre_flag = -1;
			}
			// +l
			else if (flag == 1)
			{
				//param error
				if (param.empty())
					response += makeCRLF(ERR_NOOPPARAM(client.getNickname(), ch_name, modes[i], "limit", "limit"));

				if (param.length() > 18)
					param = "0";

				long long user_limit = atoll(param.c_str());

				if (user_limit < p_channel->getUsers().size())
					continue;
				
				if (p_channel->findMode(modes[i]) && user_limit == p_channel->getUserLimit())
					continue;

				p_channel->setUserLimit(user_limit);
				p_channel->addMode(modes[i]);
				if (pre_flag == -1 || pre_flag == 0)
					ch_modes += "+";
				ch_modes += modes[i];
				ch_params += " " + param;
				pre_flag = 1;
			}
		}
		else if (modes[i] == 'o')
		{
			std::string param;
			std::map<std::string, Client> ch_users = p_channel->getUsers();
			buffer_stream >> param;

			// param error
			if (param.empty())
				response += makeCRLF(ERR_NOOPPARAM(client.getNickname(), ch_name, modes[i], "op", "nick"));
			// op error
			else if (!p_channel->isOperator(client))
				response += makeCRLF(ERR_CHANOPRIVSNEEDEDMODE(client.getNickname(), ch_name, modes[i]));
			// no such nick error
			else if (ch_users.find(param) == ch_users.end())
			{
				response += makeCRLF(ERR_NOSUCHNICK(client.getNickname(), param));
			}
			// +o
			else if (flag == 1)
			{
				Client to;
				to = getClientByName(to, param);
				if (p_channel->isOperator(to))
					continue;

				p_channel->setOperator(to);
				if (pre_flag == -1 || pre_flag == 0)
					ch_modes += "+";
				ch_modes += modes[i];
				ch_params += " " + param;
				pre_flag = 1;
			}
			// -o
			else if (flag == -1)
			{
				Client to;
				to = getClientByName(to, param);
				if (!p_channel->isOperator(to))
					continue;

				p_channel->changeAuth(COMMON, to);
				if (pre_flag == 1 || pre_flag == 0)
					ch_modes += "-";
				ch_modes += modes[i];
				ch_params += " " + param;
				pre_flag = -1;
			}
		}
		else if (modes[i] == 'p' || modes[i] == 's' || modes[i] == 'v' || modes[i] == 'm' || modes[i] == 'b')
			continue;
		else
			response += makeCRLF(ERR_UNKNOWNMODE(client.getNickname(), modes[i]));
	}

	if (ch_params.empty() && !ch_modes.empty())
		ch_modes.insert(0, ":");
	else
	{
		std::string::size_type tmp = ch_params.rfind(" ");
		ch_params.insert(tmp + 1, ":");
	}

	if (!ch_modes.empty())
		broadcast(ch_name, RPL_MODE(client.getPrefix(), ch_name, ch_modes, ch_params));

	return response;
}

std::string Server::getChannelModeResponse(Client& client, Channel* p_channel)
{
	std::string reply;
	std::string response;
	std::string ch_modes = "+";
	std::string ch_params = "";
	std::string ch_name = p_channel->getName();

	int cnt = 0;
	int key = 0;

	if (p_channel->findMode('k'))
		key++;
	if (p_channel->findMode('l'))
		key++;

	std::set<char> modes = p_channel->getModes();

	if (key == 0)
			ch_modes = ":+";
	for (std::set<char>::iterator m_it = modes.begin(); m_it != modes.end(); m_it++)
	{
		if (*m_it == 'k')
		{
			if (cnt == key - 1)
				ch_params += " :";
			else
				ch_params += " ";
			ch_params += p_channel->getPassword();
			cnt++;
		}
		else if (*m_it == 'l')
		{
			if (cnt == key - 1)
				ch_params += " :";
			else
				ch_params += " ";
			ch_params += std::to_string(p_channel->getUserLimit());
			cnt++;
		}

		ch_modes += *m_it;
	}
	response = makeCRLF(RPL_CHANNELMODEIS(client.getNickname(), ch_name, ch_modes, ch_params));
	response += makeCRLF(RPL_CHANNELCREATETIME(client.getNickname(), ch_name, std::to_string(channels[ch_name]->getCreateTime())));

	return response;
}