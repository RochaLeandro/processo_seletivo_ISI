/*********************************************************
* Tarefa para processo seletivo ISI
* Por: Leandro Pohlmann Rocha
*********************************************************/


/* Standard includes. */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <math.h>
#include <string.h>

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* BUFFER SIZE */
#define BUFFER_SIZE 1000

/* Handles Filas */
QueueHandle_t xADCQueue;
QueueHandle_t xProcessedQueue;

/* Prototipos de Tarefas*/
void vTaskADC(void *pvParameters);
void vTaskProcessing(void *pvParameters);
void vTaskUI(void *pvParameters);
void vTaskRunStatus(void *pvParameters);

void printData();
void clearQueues();

/* Semaforo ADCMutex */
SemaphoreHandle_t ADCMutex;
SemaphoreHandle_t DataMutex;

/*Handles das Terefas*/
TaskHandle_t xTaskADCHandle;
TaskHandle_t xTaskProcessingHandle;
TaskHandle_t xTaskUIHandle;
TaskHandle_t xTaskRunStatusHandle;


int main(void)
{
    BaseType_t xTaskStatus;

    ADCMutex = xSemaphoreCreateMutex();
    DataMutex = xSemaphoreCreateMutex();

    xADCQueue = xQueueCreate(BUFFER_SIZE,sizeof(float)); 
    xProcessedQueue = xQueueCreate(BUFFER_SIZE,sizeof(float)); 

    if(xADCQueue == NULL){
        printf("Nao foi possível criar a fila do ADC\r\n");
        while(1);
    }

    if(xProcessedQueue == NULL){
        printf("Nao foi possível criar a fila de processamento\r\n");
        while(1);

    }
    xTaskStatus = xTaskCreate(&vTaskADC, "Task ADC", 1024, NULL, 1, &xTaskADCHandle);
    if(xTaskStatus == pdFAIL){
        printf("Não foi possível criar a Task ADC");
        while(1);
    }


    xTaskStatus = xTaskCreate(&vTaskProcessing, "Task Processing", 1024, NULL, 1, &xTaskProcessingHandle);
    if(xTaskStatus == pdFAIL){
        printf("Não foi possível criar a Task Processing");
        while(1);
    }

    xTaskStatus = xTaskCreate(&vTaskUI, "Task UI", 1024, NULL, 1, &xTaskUIHandle);
    if(xTaskStatus == pdFAIL){
        printf("Não foi possível criar a Task UI");
        while(1);
    }

    xTaskStatus = xTaskCreate(&vTaskRunStatus, "Task RunStatus", 1024, NULL, 1, &xTaskRunStatusHandle);

    if(xTaskStatus == pdFAIL){
        printf("Não foi possível criar a Task RunStatus");
        while(1);
    }

    vTaskStartScheduler();

    return 0;
}


void vTaskADC(void *pvParameters){
    TickType_t xLastWakeTime;
    int i = 0;
    float ADC_value,removedValue;
    while (1)
    {   
        xLastWakeTime = xTaskGetTickCount();
        vTaskDelayUntil(&xLastWakeTime,pdMS_TO_TICKS(1));
        if (i<BUFFER_SIZE){
            ADC_value= 4096 * sin(60 * 3.141592*i/BUFFER_SIZE);

            if (uxQueueSpacesAvailable(xADCQueue) > 0){
                xSemaphoreTake(ADCMutex,portMAX_DELAY);
                xQueueSend(xADCQueue, &ADC_value,portMAX_DELAY);
                //printf("1ADC %d: %f\r\n",i,ADC_value);
            }else{
                xSemaphoreTake(ADCMutex,portMAX_DELAY);
                xQueueReceive(xADCQueue,&removedValue,0);
                xQueueSend(xADCQueue, &ADC_value,portMAX_DELAY);
                //printf("2ADC %d: %f\r\n",i,ADC_value);
            }   
            xSemaphoreGive(ADCMutex);
            i++;
        }else{
            i=0;
        }
    }
}


void vTaskProcessing(void *pvParameters){
    TickType_t xLastWakeTime;
    float ADCLastValue,removedValue = 0;
    xLastWakeTime = xTaskGetTickCount();
    while(1){
        
        vTaskDelayUntil(&xLastWakeTime,pdMS_TO_TICKS(100));

        xSemaphoreTake(ADCMutex,portMAX_DELAY);
        while(xQueueReceive(xADCQueue,&ADCLastValue,0)){
            xSemaphoreGive(ADCMutex);
            ADCLastValue *= M_PI;

            xSemaphoreTake(DataMutex,portMAX_DELAY);
            if (uxQueueSpacesAvailable(xProcessedQueue) > 0){
                xQueueSend(xProcessedQueue, &ADCLastValue,portMAX_DELAY);
                //printf("1SIG: %f\r\n",ADCLastValue);
            }else{
                xQueueReceive(xProcessedQueue,&removedValue,0);
                xQueueSend(xProcessedQueue, &ADCLastValue,portMAX_DELAY);
                //printf("2SIG: %f\r\n",ADCLastValue);
            }  
            
            xSemaphoreGive(DataMutex); 
        }
    }
}

void vTaskUI(void *pvParameters){
    char userChar;
	char userString[255] = {0};
    uint8_t bufferCount = 0;
	uint8_t i = 0;

	const char processCmd[] = "obter\n";
	const char clearCmd[] = "zerar\n";

    while(1){
        userChar = getchar();
		if (userChar != -1){
            userString[bufferCount] = userChar;
            bufferCount++;
            printf("Count %d\n",bufferCount);
            if (userChar == '\n'){
                printf("1\n");
                if (bufferCount == 6){
                    printf("Count %d\n",bufferCount);
                    if (!strcmp(processCmd, &userString[bufferCount-6])){                        
                            printf("Reading processed data...\r\n");
                            printData();
                            printf("Done!\r\n");
                    }
                    else{
                        if (!strcmp(clearCmd, &userString[bufferCount-6])){
                            printf("Clearing buffers...\n");
                            clearQueues();
                            printf("Done!\n");
                        }else{
                            printf("Unknown command!\n");
                        }                      
                    }
                }else{
                    printf("Unknown command!\n");
                    }

                /* Reset user input string */
                for (i = 0; i < bufferCount; i++)
                {
                    userString[i] = 0;
                }
                /* Reset user input counter */
                bufferCount = 0;
            }
        }
    }
}

void vTaskRunStatus(void *pvParameters){
     while (1)
    {
        /* code */
    }

}

void printData(){

    float value;

    xSemaphoreTake(DataMutex,portMAX_DELAY);    
    while(xQueueReceive(xProcessedQueue,&value,0)){
        printf("%f\n", value);            
    }
    xSemaphoreGive(DataMutex); 
}

void clearQueues(){
    float value;

    xSemaphoreTake(ADCMutex,portMAX_DELAY);    
    while(xQueueReceive(xADCQueue,&value,0));    
    xSemaphoreGive(DataMutex); 

    xSemaphoreTake(DataMutex,portMAX_DELAY);    
    while(xQueueReceive(xProcessedQueue,&value,0));
    xSemaphoreGive(DataMutex); 
}
