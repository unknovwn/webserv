#include "context_server.hpp"

int main()
{
  server                server_1; // содержит vector location-ов

  location location_1; // один из location-ов

      std::vector<std::string> index_files; // вектор индексов внутри location

      index_files.emplace_back("index.php"); // index 1
      index_files.emplace_back("index.html"); // index 2

  location_1.SetIndex(index_files); // установили в location

  std::vector<location> location_vec(1, location_1); // vector location
  server_1.SetRoutes(location_vec);

  std::cout << server_1.GetRoutes(0).GetIndex(0) << std::endl;

  return 0;
}