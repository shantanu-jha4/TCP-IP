//
//  server.h
//  server_client
//
//  Created by Shantanu Jha on 7/22/16.
//  Copyright © 2016 BlueBall. All rights reserved.
//

#ifndef server_h
#define server_h

/* Header files required for server client application
 * servient.h
 * Author: Shantanu Jha
 * Version: 1.0
 */

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netdb.h>
#include<sys/wait.h>
#include<arpa/inet.h>
#include<signal.h>
#include<netinet/in.h>
#include<errno.h>
#include<unistd.h>

#include<assert.h>

#define PORT "6969"
const int INET_ADDRLEN = 16;
const int INET_ADDRLEN6 = 46;
const int MAXDATASIZE = 100;
const int BACKLOG = 10;



#endif /* server_h */
