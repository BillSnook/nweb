/*
 * systemMonitor.cpp
 *
 *  Created on: Mar 8, 2015
 *      Author: bill
 */


#include "systemMonitor.h"

using namespace std;

int UdpClient::connectUdp(const string node,  const string service) {
  struct addrinfo hints;
  hints.ai_flags = 0;
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_protocol = 0;
  hints.ai_addrlen = 0;
  hints.ai_addr = NULL;
  hints.ai_canonname = NULL;
  hints.ai_next = NULL;
  struct addrinfo *res;
  int err_code;
  err_code = getaddrinfo(node.c_str(), service.c_str(), &hints, &res);
  if (err_code != 0) {
    cerr << "getaddrinfo failed: " << gai_strerror(err_code) << endl;
    exit(EXIT_FAILURE);
  }

  struct addrinfo *p_ai = res;
  /* loop through the result list of addrinfo and
   * try connecting each element until connection succeeds or all elements are tested */
  bool success = false;
  while (p_ai != NULL) {
    sfd = socket(p_ai->ai_family, p_ai->ai_socktype, IPPROTO_UDP);
    if (sfd == -1) { //try next if any left
      p_ai = p_ai->ai_next;
      continue;
    }
    if (connect(sfd, p_ai->ai_addr, p_ai->ai_addrlen) == 0) {
      success = true;
      break; // success
    }
    p_ai = p_ai->ai_next;
    close(sfd);
  }
  if (!success) {
    cerr << "couldn't create socket" << endl;
    return(-1);
  }

  freeaddrinfo(res);
  connected = true;
  return 0;
}

bool UdpClient::isConnected() {
  return connected;
}

int UdpClient::writeData(const string &data) {
  if (!connected)
	  return -1;
  write(sfd, data.c_str(), data.length());
  	  return 0 ;
}

int UdpClient::writeData(const char* data) {
  if (!connected)
	  return -1;
  writeData(string(data));
  	  return 0;
}

int UdpClient::writeData(const stringstream &data) {
  if (!connected)
	  return -1;
  write(sfd, data.str().c_str(), data.str().length());
  	  return 0;
}



