/*
 * Pipes.h
 *
 *  Created on: Jul 19, 2013
 *      Author: kiesej
 */

#ifndef PIPES_H_
#define PIPES_H_


#include <sys/types.h>
#include <unistd.h>
#include <poll.h>
#include <vector>

template<class T>
class IPCPipe{
public:
IPCPipe(){
  fds[0].events = POLLRDNORM | POLLIN;
  fds[1].events = POLLOUT | POLLWRBAND;;
  pipe(pfds);
}
  ~IPCPipe(){close(pfds[0]);close(pfds[1]);}

  T pwrite(T c){return write(pfds[1], &c , 256);}
  T pread(){read(pfds[0], buf, 256);return buf[0];}

  int preadready(){
    fds[0].fd=pfds[0];
    fds[1].fd=pfds[1];
    poll(fds, 2, 100);
    if (fds[0].revents & (POLLRDNORM | POLLIN))
      return 1;
    else
      return 0;
  }


private:
  int pfds[2];
  struct pollfd fds[2];
  T buf[256];

};

template<class T>
class IPCPipes{
public:
  IPCPipes(){size_=0;}
  IPCPipes(size_t Size){size_=Size;openPipes();}
  ~IPCPipes(){if(size_!=0)closePipes();}

  void open(size_t numPipes){size_=numPipes;openPipes();}
  size_t size(){return size_;}

  IPCPipe<T> * get(size_t i){if(i<size_) return ipcs_[i]; else return 0;}
  // size_t pollAll(int timeout=100)

  private:
  size_t size_;
  std::vector<IPCPipe<T> *> ipcs_;
  void openPipes(){
    for(size_t i=0;i<size_;i++){
      ipcs_.push_back(new IPCPipe<T>);
    }
  }
  void closePipes(){
    for(size_t i=0;i<ipcs_.size();i++)
      if(ipcs_.at(i)) delete ipcs_.at(i);
  }
};



#endif /* PIPES_H_ */
