#include <ndn-cxx/face.hpp>
#include <ndn-cxx/interest.hpp>
#include <ndn-cxx/data.hpp>
#include <ndn-cxx/security/key-chain.hpp>

#include <ndn-cxx/util/time.hpp>

#include <iostream>
#include <string>

class Server
{
public:
  Server(ndn::Face& face)
    : m_face(face)
  {
    m_face.setInterestFilter("/astri/message",
                             std::bind(&Server::onInterest, this, _2),
                             std::bind([] {
                                 // std::cerr << "Prefix registered" << std::endl;
                               }),
                             [] (const ndn::Name& prefix, const std::string& reason) {
                               throw std::runtime_error("Failed to register prefix: " + reason);
                             });
  }

private:
  void
  onInterest(const ndn::Interest& interest)
  {
    std::cerr << "<< interest for " << interest << std::endl;
    /*
    if (interest.getName().size() = ) {
      // should return or print some error
      return;
    }
    */
    std::cerr << "[DEBUG]" << interest.getName() << std::endl;
    std::cerr << "[DEBUG]RIGHT BEFORE PAGE" << std::endl;

    std::string page =
      "<!DOCTYPE HTML>\n"
      "<html>\n"
      "  <head>\n"
      "    <title>Mega News</title>\n"
      "  </head>\n"
      "  <body>\n"
      "    <h1>Mega News for " + ndn::time::toString(ndn::time::system_clock::now(), "%Y-%m-%d") + "</h1>\n"
      "\n"
      "    <ul>\n"
      "      <li>Demo News</li>\n"
      "    </ul>\n"
      "\n"
      "    <h2>Today's weather</h2>"
      "    <div id=\"weather\" src=\"ndn:/demo/weather\">\n"
      "    </div>\n"
      "  </body>\n"
      "</html>";

    ndn::Name dataName("/astri/message");
    dataName.append(ndn::time::toString(ndn::time::system_clock::now(), "%Y-%m-%d"));
    dataName.appendVersion();
    dataName.appendSegment(0);

    std::shared_ptr<ndn::Data> data = std::make_shared<ndn::Data>(dataName);
    data->setContent(reinterpret_cast<const uint8_t*>(page.c_str()), page.size());
    data->setFreshnessPeriod(ndn::time::seconds(1));

    // sign data packet
    m_keyChain.sign(*data);

    //std::cerr << "[DEBUG DATA PUT]" << std::endl;
    // make data packet available for fetching
    m_face.put(*data);
  }

private:
  ndn::Face& m_face;
  ndn::KeyChain m_keyChain;
};

int
main(int argc, char** argv)
{
  try {
    // create Face instance
    ndn::Face face;

    // create server instance
    Server server(face);

    // start processing loop (it will block forever)
    face.processEvents();

    std::cerr << "After processEvent" << std::endl;
  }
  catch (const std::exception& e) {
    std::cerr << "ERROR: " << e.what() << std::endl;
  }
  return 0;
}
