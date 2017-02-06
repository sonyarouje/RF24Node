#include <nan.h>
#include <v8.h>
#include <RF24.h>
#include <RF24Network.h>
#include <iostream>
#include <ctime>
#include <stdio.h>
#include <time.h>
#include <string>
using namespace Nan;
using namespace v8;

RF24 radio(RPI_V2_GPIO_P1_15, BCM2835_SPI_CS0, BCM2835_SPI_SPEED_8MHZ);
RF24Network network(radio);

Nan::Callback *cbPeriodic;
uv_async_t* async;

struct payload_t {                  // Structure of our payload
  char msg[24];
};

struct payload_pi {
  uint16_t fromNode;
  char msg[24];
};

//--------------------------------------------------------------------------
//Below functions are just replica of RF24Network functions.
//No need to use these functions in you app.
NAN_METHOD(BeginRadio) {
  radio.begin();
}

NAN_METHOD(BeginNetwork){
  uint16_t channel = info[0]->Uint32Value();
  uint16_t thisNode = info[0]->Uint32Value();
  network.begin(channel,thisNode);
}

NAN_METHOD(Update) {
  network.update();
}

NAN_METHOD(Available) {
  v8::Local<v8::Boolean> status = Nan::New(network.available());
  info.GetReturnValue().Set(status);
}

NAN_METHOD(Read) {
  payload_t payload;
  RF24NetworkHeader header;
  network.read(header,&payload,sizeof(payload));
  info.GetReturnValue().Set(Nan::New(payload.msg).ToLocalChecked());
}
//--------------------------------------------------------------------------------

NAN_METHOD(Begin){
  if (info.Length() < 2)
      return Nan::ThrowTypeError("Should pass Channel and Node id");

  uint16_t channel = info[0]->Uint32Value();
  uint16_t thisNode = info[1]->Uint32Value();

	radio.begin();
	delay(5);
	network.begin(channel, thisNode);
}

NAN_METHOD(Write){
  if (info.Length() < 2)
      return Nan::ThrowTypeError("Should pass Receiver Node Id and Message");  

  uint16_t otherNode = info[0]->Uint32Value();
  v8::String::Utf8Value message(info[1]->ToString());
  std::string msg = std::string(*message);
  payload_t payload;
  strncpy(payload.msg, msg.c_str(),24);
  
  RF24NetworkHeader header(otherNode);
  bool ok = network.write(header,&payload, sizeof(payload));
  info.GetReturnValue().Set(ok);
}


void keepListen(void *arg) {
	while(1)
	{
		network.update();
		while (network.available()) {     // Is there anything ready for us?
			  RF24NetworkHeader header;
   			payload_t payload;
  			network.read(header,&payload,sizeof(payload));

        payload_pi localPayload;
        localPayload.fromNode = header.from_node;
        strncpy(localPayload.msg, payload.msg, 24);
        async->data = (void *) &localPayload;
        uv_async_send(async);
		}
		delay(2000);
	}
}

void doCallback(uv_async_t *handle){
  payload_pi* p = (struct payload_pi*)handle->data;
  v8::Handle<v8::Value> argv[2] = {
      Nan::New(p->fromNode),
      Nan::New(p->msg).ToLocalChecked()
    };
  cbPeriodic->Call(2, argv);
}  

NAN_METHOD(ReadAsync){
  if (info.Length() <= 0)
    return Nan::ThrowTypeError("Should pass a callback function");
  if (info.Length() > 0 && !info[0]->IsFunction())
      return Nan::ThrowTypeError("Provided callback must be a function");

  cbPeriodic = new Nan::Callback(info[0].As<Function>());
  async = (uv_async_t*)malloc(sizeof(uv_async_t));
  uv_async_init(uv_default_loop(), async, doCallback);
  uv_thread_t id;
  uv_thread_create(&id, keepListen, NULL);
  uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}

NAN_METHOD(PrintDetails) {
  radio.printDetails();
}

NAN_METHOD(Close){
  uv_close((uv_handle_t*) &async, NULL);
}


NAN_MODULE_INIT(Init){
    Nan::Set(target, New<String>("beginRadio").ToLocalChecked(),
        GetFunction(New<FunctionTemplate>(BeginRadio)).ToLocalChecked());

    Nan::Set(target, New<String>("beginNetwork").ToLocalChecked(),
        GetFunction(New<FunctionTemplate>(BeginNetwork)).ToLocalChecked());

    Nan::Set(target, New<String>("update").ToLocalChecked(),
        GetFunction(New<FunctionTemplate>(Update)).ToLocalChecked());

    Nan::Set(target, New<String>("printDetails").ToLocalChecked(),
        GetFunction(New<FunctionTemplate>(PrintDetails)).ToLocalChecked());

    Nan::Set(target, New<String>("available").ToLocalChecked(),
        GetFunction(New<FunctionTemplate>(Available)).ToLocalChecked());

    Nan::Set(target, New<String>("read").ToLocalChecked(),
        GetFunction(New<FunctionTemplate>(Read)).ToLocalChecked());         

    Nan::Set(target, New<String>("readAsync").ToLocalChecked(),
        GetFunction(New<FunctionTemplate>(ReadAsync)).ToLocalChecked());

    Nan::Set(target, New<String>("write").ToLocalChecked(),
        GetFunction(New<FunctionTemplate>(Write)).ToLocalChecked());        

    Nan::Set(target, New<String>("close").ToLocalChecked(),
        GetFunction(New<FunctionTemplate>(Close)).ToLocalChecked()); 

    Nan::Set(target, New<String>("begin").ToLocalChecked(),
        GetFunction(New<FunctionTemplate>(Begin)).ToLocalChecked());                           
}

NODE_MODULE(nrf24Node, Init)
