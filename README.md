# WasteInformationClient

Software writen for ESP8266

## Hardwareinterfaces
- push buttons
  - reset button
  - flash button
- webinterface in config mode

## Flashing
- click upload in your ide
- push buth buttons
- leave the reset botton and keep the flash botton push of on e more second
- leave the flash button
- wait until the ide finished the flashing process and the blue chip led stopped flashing
- push the reset button the esp starts executing the newly flashed firmware

## Usage after flashing
- Normal mode (requesting mode)
  - Just power the board over a USB type c cable
- Configuration mode (setting the necessary parameters)
  - push the flash button in the first second after powering the board

## Normal mode
- the board connects to the set wifi
  - the blue led on the chip light up while trying to connect to the wifi
  - if the connection fails the third led starts fadeing purple
- if a stable wifi connection could be astablished the board connects to the mqtt server to be able the send requests and receive data
- error codes:
  - white fading
    - the decive is not configured on the server
    - click on the devices menu in the web interface and set the cities, wastetypes and zones the device should get informed about
  - red fading
    - no answer to the sent request
    - make sure that the server jar is running and the board is able to connect
  - purple fading
    - the wifi connection could be established or dropped
    - make sure the set wifi options (ssid and password) are correct and the wifi is in range
    - reset the board by pushing the reset button to esablish a reconnect
## Config mode
- blue led flashes while esp is in this mode
- esp creats a wifi accespoint
  - ssid: Wastinfoboard+[deviceid]
  - password: waste123
- use a device (phone, notebook, ...) to connect to it
- use your favourite web browser and brows to 192.168.4.1
- on the shown config page you are able to set all the needed parameters
  - ssid
  - wifipassword
  - mqttserverip
  - mqttserverport
  - deviceid
  - after edited a parameter press the "Submit" button to store the setting
  - to use the new parameter(s) press the reset button the totally reset the board
  - after reseting the board is in requesting mode and trys to operate with the newly set parameters

