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
#include "TCPSocket.h"
#include "EthernetInterface.h"
#include "TreasureData.h"

#define BUFF_SIZE 100

int main(void) {
  printf("\r\nTreasure Data REST API Demo\n");

  // Connect with Ethernet
  EthernetInterface network;
  if (network.connect() != 0) {
    printf("Cannot connect to the network\n");
    return 1;
  }

  printf("Connected to the network\n\n");
  printf("MAC: %s\n", network.get_mac_address());
  printf("IP: %s\n", network.get_ip_address());
  printf("Netmask: %s\n", network.get_netmask());
  printf("Gateway: %s\n", network.get_gateway());

  // Create Treasure data objects (Network, Database, Table, APIKey)
  TreasureData* heap  = new TreasureData(&network, MBED_CONF_APP_TD_DATABASE, "heap_info",  MBED_CONF_APP_TD_APIKEY);
  TreasureData* cpu   = new TreasureData(&network, MBED_CONF_APP_TD_DATABASE, "cpu_info",   MBED_CONF_APP_TD_APIKEY);
  TreasureData* stack = new TreasureData(&network, MBED_CONF_APP_TD_DATABASE, "stack_info", MBED_CONF_APP_TD_APIKEY);
  TreasureData* sys   = new TreasureData(&network, MBED_CONF_APP_TD_DATABASE, "sys_info",   MBED_CONF_APP_TD_APIKEY);

  // Device Information Objects
  mbed_stats_cpu_t   cpuinfo;
  mbed_stats_heap_t  heapinfo;
  mbed_stats_stack_t stackinfo;
  mbed_stats_sys_t   sysinfo;

  char buff[BUFF_SIZE];

  // Get device health data, send to Treasure Data every 10 seconds
  while (1) {
    {
      // Collect local data
      mbed_stats_cpu_get(&cpuinfo);

      // Construct strings to send
      sprintf(buff,
              "{\"uptime\":%lld,\"idle_time\":%lld,\"sleep_time\":%lld,\"deep_sleep_time\":%lld}",
              cpuinfo.uptime,
              cpuinfo.idle_time,
              cpuinfo.sleep_time,
              cpuinfo.deep_sleep_time);

      // Send data to Treasure data
      printf("\r\n Sending CPU Data: '%s'\r\n", buff);
      cpu->send_data(buff, strlen(buff));
    }
    {
      mbed_stats_heap_get(&heapinfo);

      sprintf(buff,
              "{\"current_size\":%d,\"max_size\":%d,\"total_size\":%d,\"reserved_size\":%d,\"alloc_cnt\":%d,\"alloc_fail_cnt\":%d}",
              heapinfo.current_size,
              heapinfo.max_size,
              heapinfo.total_size,
              heapinfo.reserved_size,
              heapinfo.alloc_cnt,
              heapinfo.alloc_fail_cnt);

      printf("\r\n Sending Heap Data: '%s'\r\n", buff);
      heap->send_data(buff, strlen(buff));
    }
    {
      mbed_stats_stack_get(&stackinfo);

      sprintf(buff,
              "{\"thread_id\":%d,\"max_size\":%d,\"reserved_size\":%d,\"stack_cnt\":%d}",
              stackinfo.thread_id,
              stackinfo.max_size,
              stackinfo.reserved_size,
              stackinfo.stack_cnt);

      printf("\r\n Sending Stack Data: '%s'\r\n", buff);
      stack->send_data(buff, strlen(buff));
    }
    {
      mbed_stats_sys_get(&sysinfo);

      sprintf(buff,
              "{\"os_version\":%d,\"cpu_id\":%d,\"compiler_id\":%d,\"compiler_version\":%d}",
              sysinfo.os_version,
              sysinfo.cpu_id,
              sysinfo.compiler_id,
              sysinfo.compiler_version);

      printf("\r\n Sending System Data: '%s'\r\n", buff);
      sys->send_data(buff, strlen(buff));
    }

    wait(10);
  }

  network.disconnect();

  printf("\nDone\n");
}
