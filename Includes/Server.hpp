/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hchakoub <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/08 15:03:44 by hchakoub          #+#    #+#             */
/*   Updated: 2023/05/02 13:23:31 by hchakoub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef  SERVER_HPP
#define SERVER_HPP

#include "Tockenizer.hpp"
#include <cstddef>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sys/socket.h>
#include <iostream>
#include <fcntl.h>
#include <netdb.h>

class Location
{
	public:
		typedef void (Location::*memberPointer)(const std::string&);

	private:
		std::string location_;
		std::vector<std::string> allowed_methods_;
		std::string redirection_;
		std::string root_;
		bool auto_index_;
		std::vector<std::string> indexs_;
		std::map<std::string, std::string> cgis_;
		std::string upload_dir_;
		Tockenizer *tockenizer_;


	public:

    /*
     * location constructors
     */
		Location();
		Location(const std::string &location, const std::string& locationScope);
		~Location();

    /*
     * location public data
     */
		static std::map<std::string, memberPointer> location_members_;

    /*
     * location setters
     */
		void setIndex(const std::string& val);
		void setCgi(const std::string& val);
		void setRedirection(const std::string& val);
		void setAutoIndex(const std::string& val);
		void setAllowedMethods(const std::string& val);
		void setRoot(const std::string& val);
		void setProp(const std::string& prop, const std::string& val);
		static void setMembers();


    /*
     * location getters
     */
		std::string getIndex() const;
		std::map<std::string, std::string> getCgi() const;
		std::string getRedirection() const;
		bool getAutoIndex() const;
    std::string getRoot() const;

    /*
    * checkers
    */

    bool hasCgi() const;
		void	parse();
};

class Server
{
	public:
		typedef void (Server::*memberPointer)(const std::string&);
    typedef std::map<std::string, Location*> location_type;
    typedef std::map<std::string, Location*>::iterator location_iterator;

		private:
		std::string server_string_;
		std::string root_;
		std::string host_;
		std::string port_;
		std::string server_name_;
		std::string error_page_;
		std::size_t client_body_size_limit_;
		std::vector<std::string> indexs_;
		std::map<std::string, Location*> locations_;
		std::map<std::string, memberPointer> members_;
		Tockenizer *tockenizer_;
		static void setDictionary();

	public:
		Server();
		Server(const std::string &serverString);
		~Server();

		static std::vector<std::string> dictionary_;
		static const char* pdictionary_[];
		static bool inDictinary(const std::string& token);
		void parseServer();
		void setRoot(const std::string &val);
		void setHost(const std::string &val);
		void setPort(const std::string &val);
		void setServerName(const std::string &val);
		void setErrorPage(const std::string &val);
		void setClientBodySize(const std::string &val);
		void setIndex(const std::string &index);
		void setListen(const std::string &val);
		void pushLocation(const std::string &locationString);
		void setMembers();
		void setProp(const std::string& prop, const std::string& val);

    /*
     * getters
     */

		std::string &getRoot();

		std::string &getHost();

		std::string &getPort();

		std::string &getServerName();

		std::string &getErrorPage();

		std::size_t &getClienBodySizeLimit();

		std::vector<std::string> &getIndexes();

    std::map<std::string, Location*> &getLocations();
		// only for testing 
		void test();


    /*
    * checkers
    */

    bool hasCgi() const;

	// socket related functions
	private :
		int					_hostAddrlen;
		struct sockaddr_in	_hostAddr;

	public:
		int					createSocket();
		bool				bindSocket(int sock);
		bool				listenToConnections(int sock);
		int					&getHostAddrlen();
		struct sockaddr_in	&getHostAddr();

};

#endif
