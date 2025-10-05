#include "sjf.h"

#include <stdio.h>
#include <stdlib.h>

#include "msg.h"
#include <unistd.h>



void sjf_scheduler(uint32_t current_time_ms, queue_t *rq, pcb_t **cpu_task) {
    if (*cpu_task) {
        (*cpu_task)->ellapsed_time_ms += TICKS_MS;      // Add to the running time of the application/task
        if ((*cpu_task)->ellapsed_time_ms >= (*cpu_task)->time_ms) {
            // Task finished
            // Send msg to application
            msg_t msg = {
                .pid = (*cpu_task)->pid,
                .request = PROCESS_REQUEST_DONE,
                .time_ms = current_time_ms
            };
            if (write((*cpu_task)->sockfd, &msg, sizeof(msg_t)) != sizeof(msg_t)) {
                perror("write");
            }
            // Application finished and can be removed (this is FIFO after all)
            free((*cpu_task));
            (*cpu_task) = NULL;
        }
    }
    if (*cpu_task == NULL && rq->head !=NULL) {            // If CPU is idle
        queue_elem_t *rq_atual = rq->head;
        queue_elem_t *min_element = rq->head;
        queue_elem_t *prev = NULL, *prev_min = NULL;

         while (rq_atual != NULL) {
         if(rq_atual->pcb->time_ms < min_element->pcb->time_ms) {
          min_element = rq_atual;
          prev_min = prev;
    }
      prev = rq_atual;
      rq_atual = rq_atual->next;
}
    if(prev_min) {
     prev_min->next = min_element->next;

}else{
     rq->head = min_element->next;
}
if(min_element==rq->tail) {
   rq->tail = prev_min;
}
    *cpu_task = min_element->pcb;
    free(min_element);
// Get next task from ready queue (dequeue from head)
    }
}