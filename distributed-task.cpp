#include "distributed-task.h"

namespace distributedTask {
    
    #define MESSAGE_ID_REQUEST 0
    #define MESSAGE_ID_RESPONSE 1
    #define MESSAGE_ID_DONE 2
    
    byte _incomingFace = FACE_COUNT;
    byte _outgoingFace = 0;
    byte _state = DISTRIBUTED_TASK_STATE_IDLE;
    
    void reset() {
        _state = DISTRIBUTED_TASK_STATE_IDLE;
        _incomingFace = FACE_COUNT;
        _outgoingFace = 0;
    }

    void sendAllDone(const byte taskValue, const sendTaskMessage& send){
        FOREACH_FACE(f){
            send(f, MESSAGE_ID_DONE, taskValue);
        }
    }
    
    bool respondHandled(const byte fromFace, const byte messageId, const byte messageValue, const sendTaskMessage& send){
        if (messageId == MESSAGE_ID_REQUEST) {
            send(fromFace, MESSAGE_ID_RESPONSE, messageValue);
            return true;
        }
        return false;
    }

    void sendBack(const taskHandler& handler, const byte taskValue, const sendTaskMessage& send) {
        if(_incomingFace < FACE_COUNT) {
            send(_incomingFace, MESSAGE_ID_RESPONSE, taskValue);
            _state = DISTRIBUTED_TASK_STATE_DONE;
            return;
        }
        sendAllDone(taskValue, send);
        reset();
        handler(DISTRIBUTED_TASK_OP_DONE, taskValue);
    }

    void sendAroundThenBack(const taskHandler& handler, const byte taskValue, const sendTaskMessage& send) {
        bool sent = false;
        while(sent == false && _outgoingFace < FACE_COUNT) {
            if(_outgoingFace == _incomingFace) {
                _outgoingFace = _outgoingFace + 1;
                continue;    
            }
            sent = send(_outgoingFace, MESSAGE_ID_REQUEST, taskValue);
            if(!sent){
                _outgoingFace = _outgoingFace + 1;
            }
        }
        if(!sent) {
            sendBack(handler, taskValue, send);
            return;
        }
        _state = DISTRIBUTED_TASK_STATE_PEND;
    }
    

    void begin(const sendTaskMessage& send, const taskHandler& handler, const byte startValue) {
        _incomingFace = FACE_COUNT;
        _outgoingFace = 0;
        const byte taskValue = handler(DISTRIBUTED_TASK_OP_VALUE_IN, startValue);
        sendAroundThenBack(handler, taskValue, send);
    }

    void loopIdle(const byte fromFace, const byte messageId, const byte messageValue, taskHandler& handler, sendTaskMessage& send){
        if (messageId != MESSAGE_ID_REQUEST) {
            return;
        }
        _incomingFace = fromFace;
        _outgoingFace = 0;
        byte taskValue = handler(DISTRIBUTED_TASK_OP_VALUE_IN, messageValue);
        sendAroundThenBack(handler, taskValue, send);
    }

    void loopPending(const byte fromFace, const byte messageId, const byte messageValue, const taskHandler& handler, const sendTaskMessage& send){
        if (respondHandled(fromFace, messageId, messageValue, send)) {
            return;
        }

        if (messageId != MESSAGE_ID_RESPONSE) {
            return;
        }
        _outgoingFace = _outgoingFace + 1;
        sendAroundThenBack(handler, messageValue, send);

    }

    void loopDone(const byte fromFace, const byte messageId, const byte messageValue, const taskHandler& handler, const sendTaskMessage& send){
        if (respondHandled(fromFace, messageId, messageValue, send)) {
            return;
        }
        
        if (messageId == MESSAGE_ID_DONE) {
            sendAllDone(messageValue, send);
            reset();
            handler(DISTRIBUTED_TASK_OP_DONE, messageValue);
        }
    }

    void update(const receiveTaskMessage& receive, const sendTaskMessage& send, const taskHandler& handler){
        FOREACH_FACE(f){
            byte messageId = 0;
            byte messageValue = 0;
            bool read = receive(f, &messageId, &messageValue);
            if(!read){
                continue;
            }
            if(_state == DISTRIBUTED_TASK_STATE_IDLE) {
                loopIdle(f, messageId, messageValue, handler, send);
                continue;
            }
            if(_state == DISTRIBUTED_TASK_STATE_PEND) {
                loopPending(f, messageId, messageValue, handler, send);
                continue;
            }
            if(_state == DISTRIBUTED_TASK_STATE_DONE) {
                loopDone(f, messageId, messageValue, handler, send);
                continue;
            }
        }
    }

    byte getState(){
        return _state;
    }
}