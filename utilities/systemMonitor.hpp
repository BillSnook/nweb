/*
 * systemMonitor.hpp
 *
 *  Created on: Mar 8, 2015
 *      Author: bill
 */

#ifndef SYSTEMMONITOR_H_
#define SYSTEMMONITOR_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <cstring>
#include <string>
#include <sstream>
#include <iostream>
#include <cstdlib>


using namespace std;

// Testing class ability

class UdpClient {
public:
  UdpClient(): connected(false), sfd(-1) {};

  /**
   * @brief tries to setup connection and returns 0. In case it fails returns -1
   * @param address IP address
   * @param port port
   */
  int connectUdp(const string node, const string service);

  /**
   * @brief sends data via udp in case connection is established. Otherwise it will just do nothing
   * @returns 0 on success, -1 on failure
   */
  int writeData(const string &data);

  /**
   * @brief sends data via udp in case connection is established. Otherwise it will just do nothing
   * @returns 0 on success, -1 on failure
   */
  int writeData(const char* data);

  /**
   * @brief sends data via udp in case connection is established. Otherwise it will just do nothing
   * @returns 0 on success, -1 on failure
   */
  int writeData(const stringstream &data);

  /**
   * @brief returns true if connected
   */
  bool isConnected();

private:
  bool connected;
  int sfd; // stream file descriptor
};




#endif /* SYSTEMMONITOR_H_ */
