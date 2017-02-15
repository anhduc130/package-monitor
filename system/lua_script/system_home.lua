-- This information is used by the Wi-Fi dongle to make a wireless connection to the router in the Lab
-- or if you are using another router e.g. at home, change ID and Password appropriately
SSID = "Jung"
SSID_PASSWORD = "jung1234"

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
HOST = "iot-https-relay.appspot.com" 

-- The following variable defines the TWILIO web site that we will connect to
-- use the first one if you want to send a text to a cell phone
-- use the second (commented out) one if you want to make a call to a cell phone - that's the only change
URI = "/twilio/Messages.json"
--URI = "/twilio/Calls.json"

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

function getCode(host, port, url, deviceId,callback)

    -- create a connection to the web site here
    conn=net.createConnection(net.TCP, false) 
    payloadFound = false

    -- this call back function is called when a packet of data arrives from the web site            
    conn:on("receive", function(conn, payload)
    -- optional next line shows the packets of data arriving from the web site
        print(string.len(payload))
        
        if (payloadFound == true) then
            print(payload)
        else
        -- look for the \r\n\r\n that separates an http response header from the body
            if (string.find(payload,"\r\n\r\n") ~= nil) then
                print(string.sub(payload,string.find(payload,"\r\n\r\n") + 4))
                payloadFound = true
            end
        end

        payload = nil
        collectgarbage()
    end)

    -- this call back function is called when we disconnect from the web site
    conn:on("disconnection", function(conn) 
        conn = nil
        print("Disconnected\n")
        callback("ok")
    end)
    
    -- this call back function is called when a connection to the web site is made
    conn:on("connection", function(conn)
        conn:send("GET /"..url.." HTTP/1.0\r\n"..
              "Host: "..host.."\r\n"..
              "Connection: close\r\n"..
              "Accept-Charset: utf-8\r\n"..
              "Accept-Encoding: \r\n"..
              "User-Agent: Mozilla/4.0 (compatible; esp8266 Lua; Windows NT 5.1)\r\n".. 
              "Accept: */*\r\n\r\n")
    end)
    -- connect to the web site here
    conn:connect(port,host)
end

function check_wifi()
  ip = wifi.sta.getip()

 if(ip==nil) then
   print("Connecting...")
 else
  tmr.stop(0)
  print("Connected to AP!")
  print(ip)
  end
end
