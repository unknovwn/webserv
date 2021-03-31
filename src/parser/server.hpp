#pragma once

#include <string>
#include <list>
#include <stdexcept>
#include "location.hpp"

class Server {
	public:
		typedef std::list<Location> locations_t;
		typedef std::list<std::string> server_names_t;
		typedef std::string host_t;

		Server();
		Server(const Server& other);
		~Server();

		Server& operator=(const Server& rhs);

		const locations_t& locations() const;
		template <typename InputIter>
		set_locations(InputIter first, InputIter last);

		const server_names_t& server_names() const;
		template <typename InputIter>
		set_server_names(InputIter first, InputIter last);

		const std::string& root() const;
		template <typename InputIter>
		set_roots(InputIter first, InputIter last);

		const host_t& host() const;
		template <typename InputIter>
		set_host(InputIter first, InputIter last);

		size_t client_max_body_size() const;
		template <typename InputIter>
		set_client_max_body_size(InputIter first, InputIter last);

		int port() const;
		template <typename InputIter>
		set_port(InputIter first, InputIter last);

		class InvalidHostException: public std::exception {
			public:
				virtual const char* what() const throw();
		};

		class InvalidPortException: public std::exception {
			public:
				virtual const char* what() const throw();
		}

		class InvalidServerNamesException: public std::exception {
			public:
				virtual const char* what() const throw();
		};

		class InvalidClientMaxBodySizeException: public std::exception {
			public:
				virtual const char* what() const throw();
		};

		class NoListenPortException: public std::exception {
			public:
				virtual const char* what() const throw();
		};

	private:
		locations_t    locations_;
		server_names_t server_names_;
		std::string	   root_;
		host_t         host_;
		size_t         client_max_body_size_;
		int			   port_;

};
