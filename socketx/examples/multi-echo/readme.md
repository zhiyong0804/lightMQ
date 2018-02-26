## Multi-echo

Multi-echo is a simple example for testing socketx. The Multi-echo has the following characteristics based on [cmu/440/p0](https://github.com/p0/blob/master/p0.pdf):

- The server manages its clients concurrently by allocating threads for each client.

- The server assumes that all messages are line-oriented, separated by newline (\n) characters.

- The server sends the messages it received to all of clients.

- The server needs to be responsive to slow-reading clients. As [cmu/440/p0](https://github.com/p0/blob/master/p0.pdf) mentioned, we keep a queue of at most 100 outgoing messages to be written to the client at a later time. But different from [cmu/440/p0](https://github.com/p0/blob/master/p0.pdf), our server will wait for writing messages if the queue has no space. To achieve this goal in a thread safe way, socketx provides *circular queue*.