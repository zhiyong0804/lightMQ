#include "Socket.hpp"

namespace socketx
{
 
    /*****************Class Socket************************/
    /*Constructor*/
    Socket::Socket()
    {
        hostlen = sizeof(struct sockaddr_storage);
        socketfd = -1;
    }

    Socket::~Socket()
    {
        closeConn();
    }

    /*Return the hostname and port of the host it connect currently*/
    std::string Socket::getHostname()
    {
        struct sockaddr_in addr;
        socklen_t addr_len;
        getsockname(socketfd,(struct sockaddr *)&addr,&addr_len);
        return inet_ntoa(addr.sin_addr);
    }
    
    std::string Socket::getPeername()
    {
        struct sockaddr_in addr;
        socklen_t addr_len;
        getpeername(socketfd,(struct sockaddr *)&addr,&addr_len);
        return inet_ntoa(addr.sin_addr);
    }

    size_t Socket::getPort()
    {
        struct sockaddr_in addr;
        socklen_t addr_len;
        getsockname(socketfd,(struct sockaddr *)&addr,&addr_len);
        return ntohs(addr.sin_port);
    }

    int Socket::closeConn()
    {
        return ::close(socketfd);
    }
    

    /*****************Class ServerSocket************************/
    ServerSocket::ServerSocket(EventLoop *loop, std::string port):
        loop_(loop),
        port_(port),
        event_(std::make_shared<Event>(loop,-1))
    { 
        event_->setReadFunc(std::bind(&ServerSocket::handleAccept,this));
    }

    ServerSocket::~ServerSocket()
    {

    }

    int ServerSocket::listen()
    {
        /*  Invoke C API for listening a port*/
        struct addrinfo *listp, *p;
        int rc, optval=1;
        const char *port = port_.c_str();
        /* Get a list of potential server addresses */
        memset(&hints, 0, sizeof(struct addrinfo));
        hints.ai_socktype = SOCK_STREAM;             /* Accept connections */
        hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG; /* ... on any IP address */
        hints.ai_flags |= AI_NUMERICSERV;            /* ... using port number */
        if ((rc = getaddrinfo(NULL, port, &hints, &listp)) != 0) 
        {
            fprintf(stderr, "getaddrinfo failed (port %s): %s\n", port, gai_strerror(rc));
            return -2;
        }

        /* Walk the list for one that we can bind to */
        for (p = listp; p; p = p->ai_next) 
        {
            /* Create a socket descriptor */
            if ((socketfd = ::socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0) 
                continue;  /* Socket failed, try the next */

            /* Eliminates "Address already in use" error from bind */
            setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR,    
                    (const void *)&optval , sizeof(int));

            /* Bind the descriptor to the address */
            if (bind(socketfd, p->ai_addr, p->ai_addrlen) == 0)
            {
                printf("bind succeeded\n");
                break; /* Success */
            }

            if (close(socketfd) < 0)
            { 
                /* Bind failed, try the next */
                fprintf(stderr, "open_listenfd close failed: %s\n", strerror(errno));
                return -1;
            }
        }
        freeaddrinfo(listp);
        if(!p)
        {
            printf("open listen failed");
            return -1;
        }

        if(::listen(socketfd, LISTENQ)<0)
        {
            printf("open listen failed\n");
            close(socketfd);
            return -1;
        }
        else
        {
            printf("listen succeeded\n");
            /* Regist event*/
            registEvents();
        }
        return socketfd;
    }

    int ServerSocket::accept()
    {
        int connfd = ::accept(socketfd,(struct sockaddr*)&hostaddr,&hostlen);
        if(connfd<0) printf("accept failed\n");
        else
        {
            newConnectionFunc(connfd);
        }
        return connfd;
    }

    void ServerSocket::handleAccept()
    {
        int connfd = accept();
    }

    /*****************Class ClientSocket************************/
    ClientSocket::ClientSocket(std::string hostname, std::string port)
        :hostname_(hostname),
         port_(port)
    {

    }

    ClientSocket::~ClientSocket()
    {
        
    }

    /*Start to connect to a host. 
    * This function should retry the connection
    * until the connection is established or terminated by users.
    */
    void ClientSocket::handleConnect()
    {
        socketfd = connect();
        if(socketfd < 0) exit(0);
    }

    int ClientSocket::connect()
    {
        struct addrinfo *listp, *p;
        const char *hostname = hostname_.c_str();
        const char *port = port_.c_str();

        memset(&hints, 0, sizeof(struct addrinfo));
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_NUMERICSERV;
        hints.ai_flags |= AI_ADDRCONFIG;
        getaddrinfo(hostname, port, &hints, &listp);

        for(p=listp;p;p=p->ai_next)
        {
            if((socketfd=::socket(p->ai_family,p->ai_socktype,p->ai_protocol))<0)
                continue;
            
            if(::connect(socketfd,p->ai_addr,p->ai_addrlen)!=-1)
                break;
            ::close(socketfd);
        }

        freeaddrinfo(listp);
        if(!p)
        {
            printf("connect failed\n");
            ::close(socketfd);
            return -1;    
        }
        else
        {
            printf("connect succeeded\n");
            return socketfd;
        }
    }

}
