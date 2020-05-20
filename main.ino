

#include <blinklib.h>
#include "distributed-task.h"

/*
You can implement the send functionality anyway you want. But at the end of the day, it
needs to send the two bytes of data in a way that can be consistently read by the
matching receive function. It needs to return false when it fails to send and true otherwise.
*/
bool send(const byte face, const byte messageId, const byte messageValue){
  if(isValueReceivedOnFaceExpired(face)) {
      return false;
  }
  byte data[2];
  data[0] = messageId;
  data[1] = messageValue;
  sendDatagramOnFace(&data[0], sizeof(data), face);
  return true;
}

/*
Same note as the send function, but this time the other way round 🔄
*/
bool receive(const byte face, byte* messageId, byte* messageValue){
  if(!isDatagramReadyOnFace(face)) {
      return false;
  }
  if(getDatagramLengthOnFace(face) < 2) {
      markDatagramReadOnFace(face);
      return false;
  }
  const byte* buffer = getDatagramOnFace(face);
  *messageId = buffer[0];
  *messageValue = buffer[1];
  markDatagramReadOnFace(face);
  return true;
}

/*
This is where the task work actually happens, you'll get an op code (either task data coming
in or task data done) and a value.

For the task data incoming operation, the value you return will be passed along to the next
blink in the task. The value is from the prior blink (or this blink if it began
the task). 

For the data done task, the value is the final blink's returned task value.
*/
bool _isTaskStarter;
byte countHandler(const byte op, const byte value) {
    if(op == DISTRIBUTED_TASK_OP_VALUE_IN) { //value is the number of blinks before us.
      setColor(BLUE);
      return value + 1; //there are one more of us
    }
    if(op == DISTRIBUTED_TASK_OP_DONE) { // value is the total number of blinks.
        setColor(WHITE);
        Color showColor = _isTaskStarter ? ORANGE : RED;
        _isTaskStarter = false;
        FOREACH_FACE(f){
          if(f < value){
            setColorOnFace(showColor, f);
          }
        }
    }
    return value;
}

void setup() {
	setColor(WHITE);
}

void loop() {
	if(buttonSingleClicked()){
    _isTaskStarter = true;
    distributedTask::begin(send, countHandler, 0);
  }
  distributedTask::update(receive, send, countHandler);
}