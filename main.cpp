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
#include "TreasureData.h"

#define BUFF_SIZE 100
#define TD_SEND_INTERVAL 10

C12832 lcd(SPI_MOSI, SPI_SCK, SPI_MISO, p8, p11);
Sht31 sht31(I2C_SDA, I2C_SCL);

int main(void) {
  printf("\r\nTreasure Data REST API Demo\n");

  // Connect with Ethernet
  EthernetInterface network;
  if (network.connect() != 0) {
    printf("Cannot connect to the network\n");
    return 1;
  }
  printf("Connected to the network\n\n");

  TreasureData *td = new TreasureData(&network, TD_DATABASE, "sensor_data", TD_APIKEY);

  char buff[BUFF_SIZE];

  // Get device health data, send to Treasure Data every 10 seconds
  while (1) {
    lcd.cls();

    float temp = sht31.readTemperature();
    float humidity = sht31.readHumidity();

    lcd.locate(3, 3);
    lcd.printf("Temperature: %.2f C", temp);
    lcd.locate(3, 13);
    lcd.printf("Humidity: %.2f %%", humidity);

    // Construct strings to send
    sprintf(buff, "{\"temperature\":%f,\"humidity\":%f}", temp, humidity);

    // Send data to Treasure data
    printf("\r\n Sending sensor data: '%s'\r\n", buff);
    td->send_data(buff, strlen(buff));

    wait(TD_SEND_INTERVAL);
  }

  network.disconnect();

  printf("\nDone\n");
}
