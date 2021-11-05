cmake_minimum_required(VERSION 3.10)

# set project name
project(TCP)

# add server executable
add_executable(Server server.cpp)

# link sfml libraries
target_link_libraries(Server sfml-network sfml-system)

# add client executable
add_executable(Client client.cpp)

# link sfml libraries
target_link_libraries(Client sfml-network sfml-system)