Sending Sensor Data from Mbed OS to Treasure Data
===

This example is a modified version of [BlackstoneEngineering/mbed-os-example-treasuredata-rest](https://github.com/BlackstoneEngineering/mbed-os-example-treasuredata-rest).

## Running on [mbed-simulator](https://github.com/janjongboom/mbed-simulator)

Install the simulator with [mbed-cli](https://github.com/ARMmbed/mbed-cli) and [emsdk](https://github.com/juj/emsdk):

```sh
# ...
cd /path/to/emsdk
# Follow installation instructions...
source ./emsdk_env.sh
cd /path/to/mbed-simulator
npm install
```

Set target database name and your API key at the top of [main.cpp](./main.cpp):

```cpp
const char *td_database = "TARGET_DATABASE_NAME";
const char *td_table = "sensor_data";
const char *td_apikey = "YOUR_API_KEY";
```

Place this example under `mbed-simulator/demos/`, and build all demos:

```sh
mv /path/to/mbed-os-example-treasure-data demos/treasure-data
sh build-demos.sh
```

Update `mbed-simulator/viewer/simulator.html` so you can launch the Treasure Data demo on your web browser:

```diff
diff --git a/viewer/simulator.html b/viewer/simulator.html
index d52055a..1b46d30 100644
--- a/viewer/simulator.html
+++ b/viewer/simulator.html
@@ -47,6 +47,7 @@
           <option name="ntp">Time over UDP</option>
           <option name="lorawan">LoRaWAN</option>
           <option name="lorawan-abp">LoRaWAN (ABP)</option>
+          <option name="treasure-data">Treasure Data</option>
         </select>
         <button id="load-demo">Load demo</button>
```

Launch a web server:

```sh
cd /path/to/mbed-simulator
node server.js
```

Launch the demo at: http://localhost:7829/#treasure-data

See the database on the Treasure Data console: https://console.treasuredata.com