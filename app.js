var rf24 = require('./build/Release/nrf24Node.node');

// rf24.beginRadio();
// rf24.beginNetwork(90,00);
// rf24.printDetails();
// while(true){
//     rf24.update();
//     var status = rf24.available();
//     if(status==true){
//         var data = rf24.read();
//         console.log(data);
//     }
// }

rf24.begin(90,00);
rf24.printDetails();
rf24.write(1,"Ack");

rf24.readAsync(function(from, data){
    console.log(from);
    console.log(data);
    rf24.write(1,"Ack");
});


process.on('SIGINT', exitHandler);

function exitHandler() {
    process.exit();
    rf24.write(1,"Parent ending");
    rf24.close();
}