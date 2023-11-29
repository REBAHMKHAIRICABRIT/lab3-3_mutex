/****************************************************************************
 * Copyright (C) 2020 by Fabrice Muller *
 * *
 * This file is useful for ESP32 Design course. *
 * *
 ****************************************************************************/

/**
 * @file lab3-1_main.c
 * @author Fabrice Muller
 * @date 20 Oct. 2020
 * @brief File containing the lab3-1 of Part 3.
 *
 * @see https://github.com/fmuller-pns/esp32-vscode-project-template
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include "esp_log.h"

/* FreeRTOS.org includes. */
#include "freertos/FreeRTOSConfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "my_helper_fct.h"

//static const char* TAG = "SEM";

/* Application constants */
#define STACK_SIZE 4096
#define TABLE_SIZE 400

/* Task Priority */
const uint32_t TIMER_TASK_PRIORITY = 5;
const uint32_t INC_TABLE_TASK_PRIORITY = 3;
const uint32_t DEC_TABLE_TASK_PRIORITY = 4;
const uint32_t INSPECTOR_TASK_PRIORITY = 4;
/* Communications */


/* Tasks */
void vTaskTimer(void *pvParameters);
void vTaskIncTable(void *pvParameters);
void vTaskDecTable(void *pvParameters);
void vTaskInspector(void *pvParameters);

/* Datas */
int Table[TABLE_SIZE];
SemaphoreHandle_t xSemIncTab;
SemaphoreHandle_t xSemDecTab;
/* Main function */
void app_main(void) {

      /* Init Table */
      memset(Table, 0, TABLE_SIZE*sizeof(int));

      /* Create semaphore */
      xSemDecTab = xSemaphoreCreateBinary();
      xSemIncTab = xSemaphoreCreateBinary();
      /* Stop scheduler */
      vTaskSuspendAll();

      /* Create Tasks */
      xTaskCreatePinnedToCore(      vTaskTimer,"Task 1", STACK_SIZE,(void*)"Task 1",TIMER_TASK_PRIORITY, NULL,CORE_0);  
      xTaskCreatePinnedToCore(      vTaskIncTable,"Task 2",STACK_SIZE,(void*)"Task 2",INC_TABLE_TASK_PRIORITY,NULL,CORE_0);  
      xTaskCreatePinnedToCore(      vTaskDecTable,"Task 3",STACK_SIZE,(void*)"Task 3",DEC_TABLE_TASK_PRIORITY,NULL,CORE_1);  
      xTaskCreatePinnedToCore(      vTaskInspector,"Task 3",STACK_SIZE,(void*)"Task 3",INSPECTOR_TASK_PRIORITY,NULL,CORE_1);  

      /* Continue scheduler */
      xTaskResumeAll();

      /* to ensure its exit is clean */
      vTaskDelete(NULL);
}
/*-----------------------------------------------------------*/

void vTaskTimer(void *pvParameters) {
      char *pcTaskName;
      TickType_t xLastWakeTime;
      const TickType_t xDelay250ms = pdMS_TO_TICKS(250UL);
      volatile uint32_t ul;

      pcTaskName = (char *)pvParameters;
      xLastWakeTime = xTaskGetTickCount();

      for(;;){
              DISPLAY ("Start of Timer") ;
               vTaskDelayUntil(&xLastWakeTime, xDelay250ms);
               COMPUTE_IN_TICK (2) ;
           DISPLAY ("Task Timer : given semaphore") ;
           xSemaphoreGive ( xSemDecTab ) ;
           xSemaphoreGive ( xSemIncTab ) ;
         
            }
      // for(;;){
      //       DISPLAY ("Start of Timer") ;
      //       vTaskDelay (pdMS_TO_TICKS (250));
      //       COMPUTE_IN_TICK (2) ;
      //       DISPLAY ("Task Timer : given sem") ;
      //       xSemaphoreGive ( xSemDecTab ) ;
      //       xSemaphoreGive ( xSemIncTab ) ;

      // }
       vTaskDelete(NULL);      
}

void vTaskIncTable(void *pvParameters) {
      int ActivationNumber = 0;
      for (;;) {
            xSemaphoreTake (xSemIncTab , portMAX_DELAY);
            DISPLAY ("Start of IncTable") ;
            if (ActivationNumber == 0) {
                  for (int index=0; index < TABLE_SIZE; index++) {
                        Table[index] = Table[index]+4;
                  }
                  COMPUTE_IN_TICK (5);
                  ActivationNumber = 4;
            } else {
                  ActivationNumber--;
            }
      }
      vTaskDelete(NULL);      
}

void vTaskDecTable(void *pvParameters) {
      for (;;) {
            xSemaphoreTake (xSemDecTab , portMAX_DELAY);
            DISPLAY ("Start of DecTable") ;
            for (int index=0; index < TABLE_SIZE; index++) {
                  Table[index] = Table[index]-1;
            }
            COMPUTE_IN_TICK (5);
      }
      vTaskDelete(NULL);      
}
void vTaskInspector(void *pvParameters) {


      for (;;) {
           DISPLAY ("Task Inspector is checking") ;

            int reference =Table[0];
            bool error=false;
            
            //xSemaphoreTake (xSemDecTab , portMAX_DELAY);
            for (int index=1; index < TABLE_SIZE; index++) {
                    COMPUTE_IN_TIME_US(100);
                    if(Table[index]!=(reference+index)){
                        error=true;
                    }
       
            }
           DISPLAY ("Task Inspector ended its checking") ;

          
      }
      vTaskDelete(NULL);      
}


