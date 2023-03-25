
|-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
| The use of kqueue:
|
| In general, epoll and kqueue are considered to be more efficient than select and poll as they can handle a larger number of file descriptors with better performance.
| epoll and kqueue also have better scalability than select and poll, which can be important for web servers that need to handle a large number of concurrent connections.
| In other words, you can monitor 100.000s of FDs, but the API’s execution speed only depends on the amount of events that occur in the current iteration of the event loop.
| Compared to the older poll and select APIs this is a huge improvement:
|
|    operations    |  poll  |  select   | epoll
|    10            |   0.61 |    0.73   | 0.41
|    100           |   2.9  |    3.0    | 0.42
|    1000          |  35    |   35      | 0.53
|    10000         | 990    |  930      | 0.66
|  From: The Linux programming interface (Michael Kerrisk), table 63–9
|
| That being said, macOS does not support epoll, so kqueue would be the recommended option to use for a web server on macOS.
| Kqueue is a scalable event notification mechanism provided by macOS that can be used for various types of applications, including web servers.
|
|-- select and poll use a polling model, which means that they periodically check the file descriptors for activity. In contrast, epoll and kqueue use an event-driven model, which means that they wait for events to occur and then process them.
|
|-- select and poll are limited in the number of file descriptors they can monitor at once, typically a few hundred. epoll and kqueue can handle thousands or tens of thousands of file descriptors.
|
|-- select and poll have to scan the entire list of file descriptors every time they are called, even if only a few file descriptors are active. This can cause significant CPU overhead. epoll and kqueue maintain a data structure that tracks only the active file descriptors, so they can process events more
|   efficiently.
|
|-- epoll and kqueue support edge-triggered notifications, which means that they notify the application only when there is new data available on a socket, instead of notifying the application every time there is data available. select and poll do not support edge-triggered notifications.
|
|-- epoll and kqueue provide more information about the events that occur, such as the amount of data available to be read or written, while select and poll provide only basic information about the events that occur.
|-----------------------------------------------------------------------------------------------------------------------------------------------------------------------

// use udata to store all you need // and you can use NOTE_DELETE fflags to delete a file

