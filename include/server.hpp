#pragma once

#include <string>
#include <list>
#include "location.hpp"

class Server {

	public:

	private:
		std::list<Location>    locations_;
		std::list<std::string> server_names_;
		std::list<std::string> hosts_;
		std::string	           root_;
		size_t                 client_max_body_size_;
		int			           port_;

};
