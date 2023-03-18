
#include <iostream>
// #include <sys/socket.h>
// #include <sys/types.h>
// #include <sys/event.h>
// #include <sys/time.h>
// #include <stdlib.h>
// #include <fcntl.h>

// int main()
// {
    
//     int fd = 0;
//     int fd2 = open("README.md", O_RDONLY);
//     int kq = kqueue();
//     struct kevent change[2], event[2];
//     change[0].ident = fd; // the socket(or file desctriptor) being monitored
//     change[0].filter = EVFILT_READ; // how you want to monitor the socket
//     change[0].flags = EV_ADD; // action to perform on the event
//     change[1].ident = fd; // the socket(or file desctriptor) being monitored
//     change[1].filter = EVFILT_READ; // how you want to monitor the socket
//     change[1].flags = EV_ADD; // action to perform on the event
//     int e = kevent(kq, change, 2, event, 2, NULL);
//     std::cout << "1 > " << e << std::endl;
//     // while (1)
//     // {
//          e = kevent(kq, NULL, 0, event, 2, NULL);
//         std::cout << "2 > " << e << std::endl;
//         if (e)
//             // std::cout << "ready to write" << std::endl;
//             std::cout << event[0].data << std::endl;
//             std::cout << event[1].data << std::endl;
//     // }
// }


// #include <assert.h>
// #include <stdio.h>
// #include <unistd.h>
// #include <netinet/in.h>
// #include <sys/types.h>
// #include <sys/socket.h>
// #include <sys/event.h>
// #include <sys/ioctl.h>
// #include <mach/task.h>
// #define SOCK_NONBLOCK	04000
// int kq;

// // the structure associated with a socket descriptor
// struct context {
// 	int sk;
// 	void (*rhandler)(struct context *obj);
// };

// void accept_handler(struct context *obj)
// {
// 	printf("Received socket READ event via kqueue\n");

// 	int csock = accept(obj->sk, NULL, 0);
// 	assert(csock != -1);
// 	close(csock);
// }

// int main()
// {
// 	// create kqueue object
// 	kq = kqueue();
// 	assert(kq != -1);

// 	struct context obj = {};
// 	obj.rhandler = accept_handler;

// 	// create and prepare a socket
// 	obj.sk = socket(AF_INET, SOCK_STREAM, 0);
// 	assert(obj.sk != -1);
// 	int val = 1;
// 	setsockopt(obj.sk, SOL_SOCKET, SO_REUSEADDR, &val, 4);

// 	struct sockaddr_in addr = {};
// 	addr.sin_family = AF_INET;
// 	addr.sin_port = ntohs(64000);
// 	assert(0 == bind(obj.sk, (struct sockaddr*)&addr, sizeof(addr)));
// 	assert(0 == listen(obj.sk, 0));

// 	// attach socket to kqueue
// 	struct kevent events[2];
// 	EV_SET(&events[0], obj.sk, EVFILT_READ, EV_ADD | EV_CLEAR, 0, 0, &obj);
// 	EV_SET(&events[1], obj.sk, EVFILT_WRITE, EV_ADD | EV_CLEAR, 0, 0, &obj);
// 	assert(0 == kevent(kq, events, 2, NULL, 0, NULL));

// 	// wait for incoming events from kqueue
// 	struct timespec *timeout = NULL; // wait indefinitely
// 	int n = kevent(kq, NULL, 0, events, 1, timeout);
// 	assert(n > 0);

// 	// process the received event
// 	struct context *o =(struct context *)events[0].udata;
// 	if (events[0].filter == EVFILT_READ)
// 		o->rhandler(o); // handle read event

// 	close(obj.sk);
// 	close(kq);
// }

// #include <stdlib.h>
// #include <sys/types.h>
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <arpa/inet.h>
// #include <string.h>
// #include <unistd.h>

// struct sockaddr_in create_socket_address(int port) {
//     int sockfd = socket(AF_INET, SOCK_STREAM, 0);
//     if (sockfd < 0) {
//         perror("Error opening socket");
//         exit(1);
//     }
    
//     struct sockaddr_in serv_addr;
//     bzero((char *) &serv_addr, sizeof(serv_addr));
//     serv_addr.sin_family = AF_INET;
//     serv_addr.sin_addr.s_addr = INADDR_ANY;
//     serv_addr.sin_port = htons(port);
    
//     if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
//         close(sockfd);
//         return NULL;
//     }
    
//     return serv_addr;
// }

int main() {
    // struct sockaddr_in address = create_socket_address(8080);
    // if (!address.sin_port) {
    //     printf("Bind failed!\n");
    // }
    // else {
    //     printf("Bind successful!\n");
    // }
    // return 0;
    char a = 'x';
    std::cout <<a <<std::endl;
}