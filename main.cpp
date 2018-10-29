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

EthernetInterface network;

int main(void) {
    printf("\r\nTreasure Data REST API Demo\n");

    // Connect with Ethernet
    int ret = network.connect();
    if (ret != 0) {
      printf("\nConnection error: %d\n", ret);
      return -1;
    }

    printf("Success\n\n");
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

    // Buffers to create strings in
    char cpu_buff  [BUFF_SIZE];
    char heap_buff [BUFF_SIZE];
    char stack_buff[BUFF_SIZE];
    char sys_buff  [BUFF_SIZE];

    // Get device health data, send to Treasure Data every 10 seconds
    while (1) {
      {
        // Collect local data
        mbed_stats_cpu_get(&cpuinfo);

        // Construct strings to send
        sprintf(cpu_buff,
                "{\"uptime\":%lld,\"idle_time\":%lld,\"sleep_time\":%lld,\"deep_sleep_time\":%lld}",
                cpuinfo.uptime,
                cpuinfo.idle_time,
                cpuinfo.sleep_time,
                cpuinfo.deep_sleep_time);

        // Send data to Treasure data
        printf("\r\n Sending CPU Data: '%s'\r\n", cpu_buff);
        cpu->sendData(cpu_buff, strlen(cpu_buff));
      }
      {
        mbed_stats_heap_get(&heapinfo);

        sprintf(heap_buff,
                "{\"current_size\":%d,\"max_size\":%d,\"total_size\":%d,\"reserved_size\":%d,\"alloc_cnt\":%d,\"alloc_fail_cnt\":%d}",
                heapinfo.current_size,
                heapinfo.max_size,
                heapinfo.total_size,
                heapinfo.reserved_size,
                heapinfo.alloc_cnt,
                heapinfo.alloc_fail_cnt);

        printf("\r\n Sending Heap Data: '%s'\r\n", heap_buff);
        heap->sendData(heap_buff, strlen(heap_buff));
      }
      {
        mbed_stats_stack_get(&stackinfo);

        sprintf(stack_buff,
                "{\"thread_id\":%d,\"max_size\":%d,\"reserved_size\":%d,\"stack_cnt\":%d}",
                stackinfo.thread_id,
                stackinfo.max_size,
                stackinfo.reserved_size,
                stackinfo.stack_cnt);

        printf("\r\n Sending Stack Data: '%s'\r\n", stack_buff);
        stack->sendData(stack_buff, strlen(stack_buff));
      }
      {
        mbed_stats_sys_get(&sysinfo);

        sprintf(sys_buff,
                "{\"os_version\":%d,\"cpu_id\":%d,\"compiler_id\":%d,\"compiler_version\":%d}",
                sysinfo.os_version,
                sysinfo.cpu_id,
                sysinfo.compiler_id,
                sysinfo.compiler_version);

        printf("\r\n Sending System Data: '%s'\r\n", sys_buff);
        sys->sendData(sys_buff, strlen(sys_buff));
      }

      wait(10);
    }

    network.disconnect();

    printf("\nDone\n");
}
