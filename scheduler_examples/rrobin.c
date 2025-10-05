#include "rrobin.h"

#include <stdio.h>
#include <stdlib.h>

#include "msg.h"
#include <unistd.h>
void rr_scheduler(uint32_t current_time_ms, queue_t *rq, pcb_t **cpu_task) {
    if (*cpu_task) {
        (*cpu_task)->ellapsed_time_ms += TICKS_MS;

        uint32_t slice_elapsed = current_time_ms - (*cpu_task)->slice_start_ms;

        // Processo terminou
        if ((*cpu_task)->ellapsed_time_ms >= (*cpu_task)->time_ms) {
            msg_t msg = {
                .pid = (*cpu_task)->pid,
                .request = PROCESS_REQUEST_DONE,
                .time_ms = current_time_ms
            };
            if (write((*cpu_task)->sockfd, &msg, sizeof(msg_t)) != sizeof(msg_t)) {
                perror("write");
            }

            free(*cpu_task);
            *cpu_task = NULL;
        }
        // Time slice expirou
        else if (slice_elapsed >= TIME_SLICE_MS) {
            enqueue_pcb(rq, *cpu_task); // Coloca no final da fila
            *cpu_task = NULL;
        }
    }

    // CPU está livre → escolhe próximo processo
    if (*cpu_task == NULL && rq->head != NULL) {
        *cpu_task = dequeue_pcb(rq);
        (*cpu_task)->slice_start_ms = current_time_ms;
    }
}
