#include <iostream>
#include <poll.h>
#include <vector>

int main(void)
{
    struct pollfd pfds; // More if you want to monitor more

    pfds.fd = 0;          // Standard input
    pfds.events = POLLIN; // Tell me when ready to read
	std::vector<struct pollfd> *fds = new std::vector<struct pollfd>;
	fds->push_back(pfds);

    printf("Hit RETURN or wait 2.5 seconds for timeout\n");

    int num_events = poll(&(*fds)[0], fds->size(), 2500); // 2.5 second timeout

    if (num_events == 0)
	{
        printf("Poll timed out!\n");
    }
	else
	{
        int pollin_happened = (*fds)[0].revents & POLLIN;

        if (pollin_happened) {
            printf("File descriptor %d is ready to read\n", (*fds)[0].fd);
        } else {
            printf("Unexpected event occurred: %d\n", (*fds)[0].revents);
        }
    }

    return 0;
}