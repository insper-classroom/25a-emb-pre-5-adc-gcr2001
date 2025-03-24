#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>

#include "pico/stdlib.h"
#include <stdio.h>

#include "data.h"
QueueHandle_t xQueueData;

// Tamanho da média móvel
#define MOVING_AVERAGE_SIZE 5

// Buffer para armazenar os últimos 5 dados
int moving_average_buffer[MOVING_AVERAGE_SIZE] = {0};
int moving_average_index = 0;
int moving_average_sum = 0;


// não mexer! Alimenta a fila com os dados do sinal
void data_task(void *p) {
    vTaskDelay(pdMS_TO_TICKS(400));

    int data_len = sizeof(sine_wave_four_cycles) / sizeof(sine_wave_four_cycles[0]);
    for (int i = 0; i < data_len; i++) {
        xQueueSend(xQueueData, &sine_wave_four_cycles[i], 1000000);
    }

    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void process_task(void *p) {
    int data = 0;

    while (true) {
        if (xQueueReceive(xQueueData, &data, 100)) {
            // implementar filtro aqui!
            // Subtrair o valor antigo (caso o buffer esteja cheio)
            moving_average_sum -= moving_average_buffer[moving_average_index];

            // Armazenar o novo dado no buffer
            moving_average_buffer[moving_average_index] = data;

            // Adicionar o novo valor à soma
            moving_average_sum += data;

            // Atualizar o índice para o próximo valor no buffer
            moving_average_index = (moving_average_index + 1) % MOVING_AVERAGE_SIZE;

            // Calcular a média
            int moving_average = moving_average_sum / MOVING_AVERAGE_SIZE;

            // Imprimir o dado filtrado na UART
            printf("Filtered Data: %d\n", moving_average);




            // deixar esse delay!
            vTaskDelay(pdMS_TO_TICKS(50));
        }
    }
}

int main() {
    stdio_init_all();

    xQueueData = xQueueCreate(64, sizeof(int));

    xTaskCreate(data_task, "Data task ", 4096, NULL, 1, NULL);
    xTaskCreate(process_task, "Process task", 4096, NULL, 1, NULL);

    vTaskStartScheduler();

    while (true)
        ;
}
