#include <functional>
#include <iostream>
#include <string>
#include <sstream>
#include <boost/asio.hpp>
#include <nlohmann/json.hpp>

#include "util/types.hpp"
#include "util/crypto.hpp"
#include "util/format.hpp"
#include "Database.h"

namespace net = boost::asio;
using tcp = net::ip::tcp;
using json = nlohmann::json;

class SessionCoroutine;
class Session {
  friend class SessionCoroutine;

public:
  Session(net::io_context&);

  tcp::socket& GetSocket();
  void Start();

private:
  tcp::socket m_socket;
};

// clang-format off
#include <boost/asio/yield.hpp>
class SessionCoroutine : net::coroutine {
public:
  SessionCoroutine(Session* ptr) : m_session(ptr), m_buffer(1024) {}

  void operator()(
      boost::system::error_code ec = boost::system::error_code(),
      std::size_t bytes_transferred = 0) {

    json j;
    std::string jsonString;

    if (!ec) reenter (this) {
      for (;;) {
        yield m_session->m_socket.async_read_some(net::buffer(m_buffer), *this);
        std::cout << "Read: " << bytes_transferred << std::endl;

        jsonString = std::string(reinterpret_cast<char*>(&m_buffer[0]),
                               bytes_transferred);

        j = json::parse(jsonString);
        std::cout << j["message_id"] << std::endl;

        if (j["message_id"] == "CLIENT_AUTH_REQUEST") {
          j["status"] = 200;
          j["session_salt"] = 13371337;
          jsonString = j.dump();

          yield net::async_write(m_session->m_socket, net::buffer(jsonString),
                                 *this);
        } else {
          j["status"] = 400;
          jsonString = j.dump();

          yield net::async_write(m_session->m_socket, net::buffer(jsonString),
                                 *this);
        }
      }
    }
  }

private:
  Session* m_session;
  std::vector<byte_t> m_buffer;
};
#include <boost/asio/unyield.hpp>
// clang-format on

Session::Session(net::io_context& ioContext) : m_socket(ioContext) {}
tcp::socket& Session::GetSocket() { return m_socket; }
void Session::Start() { SessionCoroutine svcCoro(this); }

class Service {
public:
  Service(net::io_context& ioContext)
      : m_ioContext(ioContext),
        m_acceptor(ioContext, tcp::endpoint(tcp::v4(), 1337)) {}

  void Run() {
    std::shared_ptr<Session> session(new Session(m_ioContext));
    m_acceptor.async_accept(session->GetSocket(),
                            std::bind(&Service::HandleAccept, this, session,
                                      std::placeholders::_1));
  }

  void HandleAccept(std::shared_ptr<Session> session,
                    const boost::system::error_code& ec) {
    if (!ec) {
      session->Start();
    }
    std::shared_ptr<Session> nextSession(new Session(m_ioContext));
    m_acceptor.async_accept(session->GetSocket(),
                            std::bind(&Service::HandleAccept, this, nextSession,
                                      std::placeholders::_1));
  }

private:
  net::io_context& m_ioContext;
  tcp::acceptor m_acceptor;
};

int main(int argc, char* argv[]) {
  std::cout << "Starting CrossZero server..." << std::endl;
  Database::GetInstance()->Open();

  net::io_context ioContext;
  Service service(ioContext);
  service.Run();

  ioContext.run();

  return 0;
}
