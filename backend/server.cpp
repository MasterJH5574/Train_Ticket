#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>
#include <cstring>

#include "commands.hpp"

#include <sstream>

const int MAXL = 1048576;

int main() {
  int a;
    for (int i = 0; true; i++) {
        std::string s;
        getline(std::cin, s);
        char recvbuf[MAXL], sendbuf[MAXL];
        memset(recvbuf, 0, sizeof(recvbuf));
        strcpy(recvbuf, s.c_str());
        std::istringstream is(recvbuf);
        std::ostringstream os(sendbuf);
        read_command(is, std::cout);
        std::cout << "\n\0";
        std::cout.flush();

        //std::cout << "What does \"" + s + "\" mean?" << std::endl;
        //std::cout.flush();
    }
    return 0;
}
