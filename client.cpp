/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright 2013,2015 Alexander Afanasyev
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <ndn-cxx/face.hpp>
#include <ndn-cxx/interest.hpp>
#include <ndn-cxx/data.hpp>

#include <iostream>
#include <string>

class Client
{
public:
  explicit
  Client(ndn::Face& face, const std::string& filename)
    : m_face(face)
    , m_baseName(ndn::Name("/bfs"))
    , m_filename(filename)
    , m_currentSeqNo(0)
  {
    
    envSetting(1);
    requestNext();
  }

private:
  int 
  envSetting(int servCount){
    std::string ip_addr;
    char command[1024];
    //char buff[1024];
    //std::string nfdc_command = "nfdc face create %s";
    if(servCount > total_server){
        return 0; 
    }
  	switch(servCount){
        case 1: ip_addr = "10.6.71.75";
                m_server = 1;
                break;

        case 2: 
                ip_addr = "10.6.71.79";
                m_server = 2;
                break;
        case 3: 
                ip_addr = "10.6.71.103";
                m_server = 3;
                break;
        case 4: 
                ip_addr = "10.6.71.113";
                m_server = 4;
                break;


    }
    sprintf(command,"nfdc route remove prefix /bfs nexthop 369; nfdc route remove prefix /bfs nexthop 371; nfdc route remove prefix /bfs nexthop 268;nfdc route remove prefix /bfs nexthop 270");
    system(command);
    sprintf(command, "nfdc face create udp4://%s;nfdc route add /bfs udp4://%s", ip_addr.c_str(),ip_addr.c_str());
    system(command);
    return 1;
    
  }


  void
  requestNext()
  {
   
    //ndn::Name nextName = ndn::Name(m_baseName).appendSequenceNumber(m_currentSeqNo);
    ndn::Name nextName = ndn::Name(m_baseName);
    nextName.append(std::to_string(m_server));
    nextName.append(m_filename);
    //std::cerr << ">> C++ " << nextName << std::endl;
    ndn::Interest nextNa(nextName);
    nextNa.setCanBePrefix(true);
    //nextNa.setInterestLifetime(2_s); // 2 seconds
    nextNa.setMustBeFresh(true);
  
    m_face.expressInterest(nextNa,
                           std::bind(&Client::onData, this, _2),
                           std::bind(&Client::onNack, this, _1),
                           std::bind(&Client::onTimeout, this, _1));
    //++m_currentSeqNo;
  }


  void
  onData(const ndn::Data& data)
  {
    std::string data_query = std::string(reinterpret_cast<const char*>(data.getContent().value()),
                                                           data.getContent().value_size());

    std::cerr << data_query
              << std::endl;

    //if(data_query[18] == 'f' && 
     if(envSetting(m_server+1)){
    //    system("ndnpeek /bfs");
        requestNext();
        }
    //}else{
    //    m_server = 0;
    //}


    //if (data.getName().get(-1).toSequenceNumber() >= 10) {
    //  return;
    //}
    
    //requestNext();
  }

  void
  onNack(const ndn::Interest& interest)
  {
    //if(m_server > total_server){
        std::cerr << "<< got NACK for " << interest << std::endl;
    //}else{
    
        if(envSetting(m_server+1))
            requestNext();
   // }
  }

  void
  onTimeout(const ndn::Interest& interest)
  {
    // re-express interest
    //if(m_server > total_server){
        std::cerr << "<< C++ timeout for " << interest << std::endl;    
    //}else{
        if(envSetting(m_server+1)){
            requestNext();
        }
    }




    /*
    m_face.expressInterest(ndn::Interest(interest.getName()),
                           std::bind(&Client::onData, this, _2),
                           std::bind(&Client::onNack, this, _1),
                           std::bind(&Client::onTimeout, this, _1));
*/

private:
  ndn::Face& m_face;
  ndn::Name m_baseName;
  uint64_t m_currentSeqNo;
  uint64_t m_server = 0;
  int total_server = 4;
  std::string m_filename;
};

int
main(int argc, char** argv)
{
  if (argc < 2) {
    std::cerr << "You have to specify filename as an argument" << std::endl;
    return -1;
  }

  try {
    //env 

    // create Face instance
    ndn::Face face;

    // create client instance
    Client client(face, argv[1]);

    // start processing loop (it will block until everything is fetched)
    face.processEvents();
  }
  catch (const std::exception& e) {
    std::cerr << "ERROR: " << e.what() << std::endl;
  }

  return 0;
}
