/* Copyright (c) 2018 ARM Limited
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

#ifndef TreasureData_H
#define TreasureData_H

#include "mbed.h"
#include "http_request.h"

#define URL_SIZE 200
#define POST_BUFFER_SIZE 200

#define TD_DEBUG false

class TreasureData {

  NetworkInterface* network;

  const char* apikey;
  const char* table;
  const char* database;

  char urlbuff[URL_SIZE];

public:

  TreasureData(NetworkInterface* network,
               const char* database,
               const char* table,
               const char* apikey) {

    this->network = network;

    this->database = database;
    this->table = table;
    this->apikey = apikey;

    // Assemble URL
    int x = sprintf(urlbuff, "http://in.treasuredata.com/postback/v3/event/%s/%s?td_write_key=%s", database, table, apikey);
    urlbuff[x] = 0; // null terminate string
    printf("\r\n Sending to: %s", urlbuff);
  }

  // Input expected to be JSON string
  // EX) "{\"key\":\"value\",}"
  int sendData(char* keyvalue, uint32_t size) {
    HttpRequest* req;
    HttpResponse* res;

    req = new HttpRequest(network, HTTP_POST, urlbuff);
    req->set_header("Content-type", "application/json");
    req->set_header("Accept", "text/plain");

    #if TD_DEBUG
      printf("\r\n Posting data: '%s'\r\n", keyvalue);
      printf("Posting Data to %s", urlbuff);
    #endif

    res = req->send(keyvalue, size);
    if (!res) {
      printf("HttpRequest failed (error code %d)\n", req->get_error());
      return 1;
    }

    #if TD_DEBUG
      printf("\n----- HTTP POST response -----\n");
      dump_response(res);
    #endif

    delete req;

    return 0;
  }

private:

#if TD_DEBUG
  void dump_response(HttpResponse* res) {
    printf("Status: %d - %s\n", res->get_status_code(), res->get_status_message().c_str());

    printf("Headers:\n");
    for (size_t ix = 0; ix < res->get_headers_length(); ix++) {
      printf("\t%s: %s\n", res->get_headers_fields()[ix]->c_str(), res->get_headers_values()[ix]->c_str());
    }
    printf("\nBody (%d bytes):\n\n%s\n", res->get_body_length(), res->get_body_as_string().c_str());
  }
#endif

};

#endif
