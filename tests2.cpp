#include <iostream>
#include <vector>
#include <redox.hpp>

using namespace std;
using redox::Redox;
using redox::Command;

double time_s() {
  unsigned long ms = chrono::system_clock::now().time_since_epoch() / chrono::microseconds(1);
  return (double)ms / 1e6;
}

int main(int argc, char* argv[]) {

  Redox rdx = {cout, redox::log::Debug};
  rdx.noWait(true);

  if(!rdx.connect("localhost", 6379)) return 1;

  if(rdx.set("simple_loop:count", "0")) {
    cout << "Reset the counter to zero." << endl;
  } else {
    cerr << "Failed to reset counter." << endl;
    return 1;
  }

  vector<string> cmd_vec = {"INCR", "simple_loop:count"};
  double freq = 1000000; // Hz
  double dt = 1 / freq; // s
  double t = 5; // s
  int parallel = 1;

  cout << "Sending \"" << rdx.vecToStr(cmd_vec) << "\" asynchronously every "
       << dt << "s for " << t << "s..." << endl;

  double t0 = time_s();
  atomic_int count(0);

  vector<Command<int>*> commands;
  for(int i = 0; i < parallel; i++) {
    commands.push_back(&rdx.commandLoop<int>(
        cmd_vec,
        [&count, &rdx](Command<int>& c) {
          if (!c.ok()) {
            cerr << "Bad reply: " << c.status() << endl;
          }
          count++;
        },
        dt
    ));
  }

  // Wait for t time, then stop the command.
  this_thread::sleep_for(chrono::microseconds((int)(t*1e6)));
  for(auto& c : commands) c->free();

  double t_elapsed = time_s() - t0;
  double actual_freq = (double)count / t_elapsed;

  // Get the final value of the counter
  long final_count = stol(rdx.get("simple_loop:count"));

  cout << "Sent " << count << " commands in " << t_elapsed << "s, "
       << "that's " << actual_freq << " commands/s." << endl;

  cout << "Final value of counter: " << final_count << endl;

  rdx.disconnect();
  return 0;
}


#include <random>
#include <iostream>
#include <functional>
#include <memory>
#include <map>
#include <algorithm>
#include <string>
#include <iostream>
#include <thread>

#include "HUtils/HStrings.hpp"
#include "Logger/Logger.hpp"
#include "Config/GlobalConf.hpp"
#include "AsyncEvent/Redis/RedisBuffer.hpp"
#include "AsyncEvent/AEvBase/AEventAbstract.hpp"
#include "AsyncEvent/Redis/AEvRedisMod.hpp"


redis::RespData result;

asio::io_service main_io;

aev::AEvStrandPtr _ev_loop(std::make_shared<asio::strand>(main_io));

aev::AEvRedisMod redis_db(_ev_loop, "127.0.0.1", 6379);

int query_counter = 0;

void ra_handler(int err, const redis::RespData &result);

void rq_handler(int err, const redis::RespData & result)
{
    if (err)
        log_debug("Error num: %", err);
      ++query_counter;
    redis_db.async_query("get test", ra_handler);

}

void ra_handler(int err, const redis::RespData & result)
{
    if (err)
        log_debug("Error num: %", err);

    if (result.ires > 900000)
        exit(0);

}

int main () {

    Config::glob().read_config("main.conf");
    if (Config::glob().have_error()) {
        CLog::glob().write("Error loading config: %", Config::glob().error_text());
        exit(0);
    }
    Config::glob().set_opt("logging_level", "3");
    RedisRBuffer buff;
    // log_main("%", sizeof(aev::AEventAbstract));
//    std::string result;

    if (!redis_db.connect())
        exit(1);

    redis_db.query_loop("incr test", [](int err, const redis::RespData & result){
        if (err || result.ires > 1000000)
            redis_db.disconnect();

    }, 1000000);

//    std::thread t ([&]() {main_io.run();});
//    std::thread t1 ([&]() {main_io.run();});
    main_io.run();
//    t.join();
//    t1.join();

}

#include <random>
#include <iostream>
#include <functional>
#include <memory>
#include <map>
#include <algorithm>
#include <string>
#include <iostream>
#include <thread>

#include "HUtils/HStrings.hpp"
#include "Logger/Logger.hpp"
#include "Config/GlobalConf.hpp"
#include "AsyncEvent/Redis/RedisBuffer.hpp"
#include "AsyncEvent/AEvBase/AEventAbstract.hpp"
#include "AsyncEvent/Redis/AEvRedisMod.hpp"


redis::RespData result;

asio::io_service main_io;

aev::AEvStrandPtr _ev_loop(std::make_shared<asio::strand>(main_io));

aev::AEvRedisMod redis_db(_ev_loop, "127.0.0.1", 6379);

int query_counter = 0;
void f(std::string const & s)
{
    std::string k {s};
}

int main () {

    log_main("%", std::thread::hardware_concurrency());
    exit(0);

    Config::glob().read_config("main.conf");
    if (Config::glob().have_error()) {
        CLog::glob().write("Error loading config: %", Config::glob().error_text());
        exit(0);
    }
    Config::glob().set_opt("logging_level", "3");

    if (!redis_db.connect())
        exit(1);

    auto ra_handler = [&redis_db](int err, const redis::RespData & result)
    {
        if (err)
            log_debug("Error num: %", err);
        // log_main("%", result.ires);
    };

    std::thread t {[&]() {main_io.run();}};

    for (int i = 1; i < 1000000; ++i)
    {
        redis_db.async_query("ping", ra_handler);
    }

    log_main("All sended.");
    redis_db.disconnect();

    t.join();
//    t1.join();

}


