
#ifndef HEADER_DISTRIBUTED_TASK
    #define HEADER_DISTRIBUTED_TASK
    
    #include <blinklib.h>
    
    namespace distributedTask {
        
        typedef byte (taskHandler)(byte op, byte value);
        typedef bool (sendTaskMessage)(byte face, byte id, byte value);
        typedef bool (receiveTaskMessage)(byte face, byte* id, byte* value);
        

        //ing: just before, ed: just after
        #define DISTRIBUTED_TASK_VALUE_IN 0
        #define DISTRIBUTED_TASK_DONE 4
        
        #define DISTRIBUTED_TASK_STATE_IDLE 0
        #define DISTRIBUTED_TASK_STATE_PEND 1
        #define DISTRIBUTED_TASK_STATE_DONE 2
        
        void begin(sendTaskMessage& send, taskHandler& handler, byte value);
        void update(receiveTaskMessage& receive, sendTaskMessage& send, taskHandler& handler);
        byte getState();
        void reset();
    }

#endif