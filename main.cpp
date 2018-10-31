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

#include "mbed.h"
#include "C12832.h"
#include "Sht31.h"
#include "EthernetInterface.h"
#include "http_request.h"

#define DEBUG false

#define URL_SIZE 200
#define BODY_SIZE 100

#define TD_SEND_INTERVAL 10

const char *td_database = "TARGET_DATABASE_NAME";
const char *td_table = "sensor_data";
const char *td_apikey = "YOUR_API_KEY";

C12832 lcd(SPI_MOSI, SPI_SCK, SPI_MISO, p8, p11);
Sht31 sht31(I2C_SDA, I2C_SCL);

void dump_response(HttpResponse *res) {
  printf("Status: %d - %s\n", res->get_status_code(), res->get_status_message().c_str());

  printf("Headers:\n");
  for (size_t ix = 0; ix < res->get_headers_length(); ix++) {
    printf("\t%s: %s\n", res->get_headers_fields()[ix]->c_str(), res->get_headers_values()[ix]->c_str());
  }
  printf("\nBody (%d bytes):\n\n%s\n", res->get_body_length(), res->get_body_as_string().c_str());
}

int main(void) {
  printf("\r\nTreasure Data REST API Demo\n");

  // Connect with Ethernet
  EthernetInterface network;
  if (network.connect() != 0) {
    printf("Cannot connect to the network\n");
    return 1;
  }
  printf("Connected to the network\n\n");

  // Assemble URL: https://support.treasuredata.com/hc/en-us/articles/360000675487-Postback-API
  char url[URL_SIZE];
  sprintf(url, "http://in.treasuredata.com/postback/v3/event/%s/%s", td_database, td_table);
  printf("\r\n Sending to: %s", url);

  char body[BODY_SIZE];

  // Get sensor data, send to Treasure Data every TD_SEND_INTERVAL seconds
  int cnt = 1;
  while (1) {
    lcd.cls();

    float temp = sht31.readTemperature();
    float humidity = sht31.readHumidity();

    lcd.locate(2, 2);
    lcd.printf("Sent %d records to TD", cnt++);
    lcd.locate(2, 12);
    lcd.printf("Temperature: %.2f C", temp);
    lcd.locate(2, 22);
    lcd.printf("Humidity: %.2f %%", humidity);

    // Construct strings to send
    sprintf(body, "{\"temperature\":%f,\"humidity\":%f}", temp, humidity);

    // Send data to Treasure data
    {
      printf("\r\n Sending sensor data: '%s'\r\n", body);

      HttpRequest *req = new HttpRequest(&network, HTTP_POST, url);
      req->set_header("Content-Type", "application/json");
      req->set_header("X-TD-Write-Key", td_apikey);

      HttpResponse *res = req->send(body, strlen(body));
      if (!res) {
        printf("HttpRequest failed (error code %d)\n", req->get_error());
        return 1;
      }

      #if DEBUG
        printf("\n----- HTTP POST response -----\n");
        dump_response(res);
      #endif

      delete req;
    }

    wait(TD_SEND_INTERVAL);
  }

  network.disconnect();

  printf("\nDone\n");
}
