#include "Loop.h"

extern bool GLOBAL_NEED_QUIT;

Loop::Loop(){

}



int Loop::loop(){


    while(true){
    
        sleep(1);
        
    	if (GLOBAL_NEED_QUIT) {
    	    break;
    	}
    }
    printf("\n");
    return 0;
}


void Loop::loop(int delay){

    sleep(delay);
    printf("\n");
    return;
}
