
#ifndef HEADER_DISTRIBUTED_TASK
    #define HEADER_DISTRIBUTED_TASK
    
    #include <blinklib.h>
    
    namespace distributedTask {
        
        typedef byte (taskHandler)(const byte op, const byte value);
        typedef bool (sendTaskMessage)(const byte face, const byte id, const byte value);
        typedef bool (receiveTaskMessage)(const byte face, byte* id, byte* value);
        
        #define DISTRIBUTED_TASK_OP_VALUE_IN 0
        #define DISTRIBUTED_TASK_OP_DONE 4
        
        #define DISTRIBUTED_TASK_STATE_IDLE 0
        #define DISTRIBUTED_TASK_STATE_PEND 1
        #define DISTRIBUTED_TASK_STATE_DONE 2
        
        void begin(const sendTaskMessage& send, const taskHandler& handler, const byte value);
        void update(const receiveTaskMessage& receive, const sendTaskMessage& send, const taskHandler& handler);
        void reset();
        byte getState();
    }

#endif