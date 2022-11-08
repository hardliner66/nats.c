// Copyright 2017-2018 The NATS Authors
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <nats.hpp>

#include <atomic>
#include <iostream>

int main(int argc, char **argv) {
  try {
    // Creates a connection to the default NATS URL
    nats::Connection conn(NATS_DEFAULT_URL);

    printf("Listening on subject 'foo'\n");

    // Creates an synchronous subscription on subject "foo".
    nats::Subscription sub = conn.SubscribeSync("foo");

    // With synchronous subscriptions, one need to poll
    // using this function. A timeout is used to instruct
    // how long we are willing to wait. The wait is in milliseconds.
    // So here, we are going to wait for 5 seconds.
    nats::Msg msg = sub.NextMsg(5000);

    std::string data(msg.GetData(), msg.GetDataLength());
    std::cout << "Received: " << msg.GetSubject() << " - " << data << std::endl;

  } catch (std::exception ex) {
    std::cout << ex.what() << std::endl;
  }

  return 0;
}
