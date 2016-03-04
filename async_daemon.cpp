#include <asio.hpp>
// #include <boost/bind.hpp>
#include "HelpfulCodes/HFiles.hpp"
#include "HelpfulCodes/HStrings.hpp"
#include "HelpfulCodes/AsyncQueue.hpp"
#include "AsioServer/AsyncSigListen.hpp"
#include "HelpfulCodes/HDaemon.hpp"

#include <iostream>
#include <fstream>
#include <functional>
#include <string>

const std::string pfilename = "/home/arti/run/async_daemon.pid";

void cbfunc(std::string s)
{
    std::cout << "Callback " << s << std::endl;
}

auto handler = [](int signal_number)->int
{
      std::cout << signal_number << std::endl;
      if (signal_number == SIGUSR1) {
          std::cout << "Stop work pid " << getpid() << std::endl;
          return AsyncSigListener::StopSigListening;
      }

      if (signal_number == SIGUSR2) {
            std::cout << "Stop work pid " << getpid() << std::endl;
      }

      if (signal_number == SIGALRM) {
            std::cout << "Stop work pid " << getpid() << std::endl;
      }

    return 0;
};


int main()
{
    if (int pid = hdaemon::pid_fread(pfilename)) {
        std::cout << "Runing proces found. Restarting pid " << getpid() << std::endl;
        kill(pid, SIGUSR1);
    }

    int pid = fork();

    if (pid < 0) {
        std::cout << "fork error" << std::endl;
        exit(1);
    }

    if (pid == 0) {


        hdaemon::pid_fwrite(pfilename, getpid());

        asio::io_service main_io;
        std::shared_ptr<asio::strand> m_strand = std::make_shared<asio::strand>(main_io);
        AsyncQueueExample aq;
        AsyncSigListener _signals(m_strand, handler);

        _signals.add(SIGHUP);
        _signals.add(SIGUSR2);
        _signals.add(SIGUSR1);
        _signals.add(SIGALRM);

        _signals.async_start();

        std::cout << "Helo World!" << std::endl;
        main_io.run();
        aq.stop();
        aq.join();

    } else {
        std::cout << "Main process exiting." << std::endl;
        return 0;
    }

}
