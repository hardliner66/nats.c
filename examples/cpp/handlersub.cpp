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

class Handler {
 public:
  Handler(const char *id) : id_(id), done(false) {}

  void msg(nats::Connection &nc, nats::Subscription &sub, nats::Msg &&msg) {
    std::string data(msg.GetData(), msg.GetDataLength());
    std::cout << id_ << " received: " << msg.GetSubject() << " - " << data << std::endl;
    done = true;
  }

  bool done;

 private:
  const char *id_;
};

int main(int argc, char **argv) {
  try {
    // Creates a connection to the default NATS URL
    nats::Connection conn(NATS_DEFAULT_URL);

    printf("Listening on subject 'foo'\n");

    Handler handler("handler");

    // Creates an asynchronous subscription on subject "foo".
    // When a message is sent on subject "foo", the callback
    // msg() will be invoked by the client library.
    nats::Subscription sub = conn.Subscribe<Handler, &Handler::msg>("foo", &handler);

    for (; !handler.done;) {
      nats::Sleep(100);
    }
  } catch (std::exception ex) {
    std::cout << ex.what() << std::endl;
  }

  return 0;
}
