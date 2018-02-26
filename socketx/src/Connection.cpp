#include "Connection.hpp"

namespace socketx{

    Connection::Connection(EventLoop *loop, int fd):
        loop_(loop),
        Socket(fd),
        event_(std::make_shared<Event>(loop,fd)),
        readFun(false),writeFun(false),closeFun(false){
        /*Set callback functions*/
        event_->setReadFunc(std::bind(&Connection::handleRead, this));
        event_->setWriteFunc(std::bind(&Connection::handleWrite, this));
        event_->setErrorFunc(std::bind(&Connection::handleError, this));
        /*Initialize internal buffer*/
        rio_readinitb(socketfd);
    }

    Connection::~Connection(){
    }

    /*Handle events*/
    void Connection::handleRead(){
        if(readFun)
            handleReadEvents(shared_from_this());
        else
            printf("No function for read events...\n");
    }

    void Connection::handleWrite(){
        if(writeFun)
            handleWriteEvents(shared_from_this());
        else
            printf("No function for write events...\n");
    }
 
    void Connection::handleError(){

    }

    void Connection::handleClose(){
        event_->deleteEvent();

        /*Tell host to delete this connection*/
        if(closeFun)
            handleCloseEvents(shared_from_this());
        else
            printf("No function for close events...\n");
    }

    /*Connect the file descriptor to rio struct*/
    void Connection::rio_readinitb(int fd){
        rio.rio_fd = fd;  
        rio.rio_cnt = 0;  
        rio.rio_bufptr = rio.rio_buf;
    }

    /* 
    * rio_read - This is a wrapper for the Unix read() function that
    *    transfers min(n, rio_cnt) bytes from an internal buffer to a user
    *    buffer, where n is the number of bytes requested by the user and
    *    rio_cnt is the number of unread bytes in the internal buffer. On
    *    entry, rio_read() refills the internal buffer via a call to
    *    read() if the internal buffer is empty.
    */
    ssize_t Connection::rio_read(char *usrbuf, size_t n){
        int cnt;

        while (rio.rio_cnt <= 0) {  /* Refill if buf is empty */
            rio.rio_cnt = read(rio.rio_fd, rio.rio_buf, 
                    sizeof(rio.rio_buf));
            if (rio.rio_cnt < 0) {
                if (errno != EINTR){/* Interrupted by sig handler return */
                    return -1;
                } 
                    
            }
            else if (rio.rio_cnt == 0)  /* EOF */
                return 0;
            else 
                rio.rio_bufptr = rio.rio_buf; /* Reset buffer ptr */
        }

        /* Copy min(n, rp->rio_cnt) bytes from internal buf to user buf */
        cnt = n;          
        if (rio.rio_cnt < n)   
            cnt = rio.rio_cnt;
        memcpy(usrbuf, rio.rio_bufptr, cnt);
        rio.rio_bufptr += cnt;
        rio.rio_cnt -= cnt;
        return cnt;
    }

    /*Send *n* bytes of buffer to the host it connected*/
    ssize_t Connection::send(const void *buffer, size_t n){
        size_t nleft = n;
        ssize_t nwritten;
        char *bufp = (char *)buffer;

        while (nleft > 0) {
            if ((nwritten = write(socketfd, bufp, nleft)) <= 0) {
                if (errno == EINTR)  /* Interrupted by sig handler return */
                    nwritten = 0;    /* and call write() again */
                else
                    return -1;       /* errno set by write() */
            }
            nleft -= nwritten;
            bufp += nwritten;
        }
        return n;
    }
 
    /*Receive bytes from the host it connected.
    * Save bytes to usrbuf with length n.
    */
    ssize_t Connection::recvFromBuffer(void *usrbuf, size_t n){
        size_t nleft = n;
        ssize_t nread;
        char *bufp = (char *)usrbuf;
        
        while (nleft > 0) {
            if ((nread = rio_read(bufp, nleft)) < 0) 
                    return -1;          /* errno set by read() */ 
            else if (nread == 0)
                break;              /* EOF */
            nleft -= nread;
            bufp += nread;
        }
        return (n - nleft);         /* return >= 0 */
    }

    /*Override function of recv.
    * This function needs a fd parameter,
    * It does not use the internal buffer.
    */
    ssize_t Connection::recv(void *usrbuf, size_t n){
        size_t nleft = n;
        ssize_t nread;
        char *bufp = (char *)usrbuf;

        while (nleft > 0) {
            if ((nread = read(socketfd, bufp, nleft)) < 0) {
                if (errno == EINTR) /* Interrupted by sig handler return */
                    nread = 0;      /* and call read() again */
                else
                    return -1;      /* errno set by read() */ 
            } 
            else if (nread == 0)
                break;              /* EOF */
            nleft -= nread;
            bufp += nread;
        }
        return (n - nleft);         /* Return >= 0 */
    }

    std::string Connection::readline(){
        int n, rc;
        char c;
        std::string str="";

        for (n = 1;; n++) { 
            if ((rc = rio_read(&c, 1)) == 1) {
                str += c;
                if (c == '\n') {
                    n++;
                    break;
                }
            } else if (rc == 0) {
                if (n == 1)
                    return ""; /* EOF, no data read */
                else
                    break;    /* EOF, some data was read */
            } else
            return "";	  /* Error */
        }
        return str;
    }
    

    /*Send and receive messages*/
    ssize_t Connection::sendmsg(const Message &msg){
        size_t n = msg.getSize();
        char * buffer = msg.getData();
        /*Send the size of the message first*/
        if(send(&n,sizeof(n))>0){
            /*Send data*/
            if(send(buffer,n)>0)
                return 1;
        }
        return -1;
    }

    Message Connection::recvmsgFromBuffer(){
        size_t n = 0;
        /*Receive the size of the message*/
        if(recvFromBuffer(&n,sizeof(size_t))>0){
            /*Recieve the message*/
            char * data = new char[n];
            recvFromBuffer(data,n);
            return Message(data,n);
        }
        else return Message(nullptr,0);
    }

     Message Connection::recvmsg(){
        size_t n = 0;
        /*Receive the size of the message*/
        if(recv(&n,sizeof(size_t))>0){
            /*Recieve the message*/
            char * data = new char[n];
            recv(data,n);
            return Message(data,n);
        }
        else return Message(nullptr,0);
    }
}