-- This information is used by the Wi-Fi dongle to make a wireless connection to the router in the Lab
-- or if you are using another router e.g. at home, change ID and Password appropriately
SSID = "M112-PD"
SSID_PASSWORD = "aiv4aith2Zie4Aeg"

-- configure ESP as a station
wifi.setmode(wifi.STATION)
wifi.sta.config(SSID,SSID_PASSWORD)
wifi.sta.autoconnect(1)

-- alternatively you could do it this way
-- wifi.sta.config("M112-PD","aiv4aith2Zie4Aeg", 1)
-- wifi.sta.connect()

-- pause for connection to take place - adjust time delay if necessary or repeat until connection made
tmr.delay(1000000) -- wait 1,000,000 us = 1 second

-- This should print 5 if connection was successful
-- print(wifi.sta.status())

-- Prints the IP given to ESP8266
-- print(wifi.sta.getip())

-- List all available wireless network ---
-- See documentation: https://nodemcu.readthedocs.io/en/master/en/modules/wifi/#wifistagetap

-- The following 2 pieces of information are related to your Twilio account the one you made in Exercise 1.9
-- change appropriately
TWILIO_ACCOUNT_SID = "AC65a547020d4e7ee0158199f32986dffa"
TWILIO_TOKEN =       "4b2191351b90e784b959b5f6d6facfec"

-- Unfortunately, the Wi-FI dongle can only make unsecured HTTP requests, but Twilio requires
-- secured HTTPS requests, so we will use a relay website to convert HTTP requests into HTTPS requests
-- visit http://iot-https-relay.appspot.com/ to learn more about this service
-- Please be sure to understand the security issues of using this relay app and use at your own risk.

-- this is the web address of the relay web site that our dongle sends the initial HTTP request to
HOST_TWILIO = "iot-https-relay.appspot.com"

-- The following variable defines the TWILIO web site that we will connect to
-- use the first one if you want to send a text to a cell phone
-- use the second (commented out) one if you want to make a call to a cell phone - that's the only change
URI_TWILIO = "/twilio/Messages.json"
--URI = "/twilio/Calls.json"

-- Web address
HOST_SERVER = "https://davidjung.co"

-- URI server
URI_SERVER = "/dashboard/api/access_code.json"

function build_post_request(host, uri, data_table)

     data = ""

     for param,value in pairs(data_table) do
          data = data .. param.."="..value.."&"
     end

     request = "POST "..uri.." HTTP/1.1\r\n"..
     "Host: "..host.."\r\n"..
     "Connection: close\r\n"..
     "Content-Type: application/x-www-form-urlencoded\r\n"..
     "Content-Length: "..string.len(data).."\r\n"..
     "\r\n"..
     data
     print(request)
     return request
end

function build_get_request(host, uri, data_table)

     data = ""

     for param,value in pairs(data_table) do
          data = data .. param.."="..value.."&"
     end

     request = "GET "..uri.." HTTP/1.1\r\n"..
     "Host: "..host.."\r\n"..
     "Connection: close\r\n"..
     "Content-Type: application/x-www-form-urlencoded\r\n"..
     "Content-Length: "..string.len(data).."\r\n"..
     "\r\n"..
     data
     print(request)
     return request
end

-- This function registers a function to echo back any response from the server, to our DE1/NIOS system
-- or hyper-terminal (depending on what the dongle is connected to)
function display(sck,response)
     print(response)
end

-- request code, requests a code from our webserver, give it the device id
function request_code(device_id)
  Gdata = {
      id = device_id,
     }

  socket = net.createConnection(net.TCP,0)
  socket:on("recieve",display)
  socket:connect(80,HOST_SERVER)

  socket:on("connection",function(sck)
  get_request = build_get_request(HOST,URI,Gdata)
  sck:send(get_request)
end


-- When using send_sms: the "from" number HAS to be your twilio number.
-- If you have a free twilio account the "to" number HAS to be your twilio verified number.
function send_sms(from,to,body)
  Pdata = {
      sid = TWILIO_ACCOUNT_SID,
      token = TWILIO_TOKEN,
      Body = string.gsub(body," ","+"),
      From = from,
      To = to
     }

     socket = net.createConnection(net.TCP,0)
     socket:on("receive",display)
     socket:connect(80,HOST)

     socket:on("connection",function(sck)
          post_request = build_post_request(HOST,URI,Pdata)
          sck:send(post_request)
     end)
end


function check_wifi()
  ip = wifi.sta.getip()

 if(ip==nil) then
   print("Connecting...")
 else
  tmr.stop(0)
  print("Connected to AP!")
  print(ip)
  -- send a text message from, to, text
  send_sms("(778) xxx-xxxx","(604) xxx-xxxx","Ring-Ring - this is your breadboard calling!!!!")

 end
end
