#RF24Node
Nodejs Addon of RF24Network library for Raspberry Pi

A nodejs addon of the RF24Network library (https://github.com/TMRh20/RF24Network) for Raspberry Pi. In my testing RF24Network is a great library to communicate with nrf24L01 modules. Most developments in my Raspberry Pi I use nodejs and this library lacks a nodejs support. So I decided to write one.

>All the credit goes to the orginal developers of RF24 and RF24Network library. I just created an addon for Nodejs.

##How to use
1. Clone RF24Node repository 'git clone https://github.com/sonyarouje/RF24Node'
2. Enter RF24Node directory 'cd RF24Node'
3. Follow the instructions given in RFNetwork's readme and clone RF24 and RF24Network directories to RaspberryPi.
4. Once the above steps complete, we can see RF24, RF24Network under RF24Node directory.
5. Issue 'npm install'
6. The new nodejs addon will created under /RF24Node/build/Release/rf24Node.node

>Note: This module created using Nodejs version 4.2.1 and node-gyp version 3.5.0. Not tested with any other version.

##APIs
begin: Accepts two parameters. Channel and Id of the device, say channel as '90' and Id as '00'--

printDetails: Print the details of the connected rf24 module.--

readAsync: Pass a callback function whick takes two parameters. First parameter will give the address of the device that send the message and second one is the message.--

write: Send a message to another module. Accepts address and message.--

>Note: Message can be upto 24 character. 

##Example
app.js
```javascript
var rf24 = require('./build/Release/nrf24Node.node');
rf24.begin(90,00);
rf24.printDetails();


rf24.readAsync(function(fromAddress, data){
    console.log(data);
    rf24.write(1,"Ack");
});
```

