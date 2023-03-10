#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 *
 * See http://www.freertos.org/a00110.html.
 *----------------------------------------------------------*/

#define configUSE_PREEMPTION                    1			/* Runs the scheduler every tick interrupt */
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 0
#define configCPU_CLOCK_HZ                     	16000000U		/* Clock tick feeding the RTOS timer */
#define configTICK_RATE_HZ                 	(TickType_t) 1000	/* Tick interrupt frequency, should be <= 1000hz */
#define configMAX_PRIORITIES                    7
#define configMINIMAL_STACK_SIZE                128
#define configMAX_TASK_NAME_LEN                 16
#define configUSE_16_BIT_TICKS                  0
#define configIDLE_SHOULD_YIELD                 0
#define configUSE_TASK_NOTIFICATIONS            1
#define configUSE_MUTEXES                       1
#define configUSE_RECURSIVE_MUTEXES             0
#define configUSE_COUNTING_SEMAPHORES           1
#define configQUEUE_REGISTRY_SIZE               10	/* Max. number of queues and semaphores that a can be registered for RTOS aware debugger */
#define configUSE_QUEUE_SETS                    1
#define configUSE_TIME_SLICING                  1	/* Switch between equal priority tasks when the scheduler runs */
#define configENABLE_BACKWARD_COMPATIBILITY     0
#define configNUM_THREAD_LOCAL_STORAGE_POINTERS 5

/* Memory allocation related definitions. */
#define configSUPPORT_STATIC_ALLOCATION         0
#define configSUPPORT_DYNAMIC_ALLOCATION        1
#define configTOTAL_HEAP_SIZE                   (10*1024)	/* Used for schemes heap_1, heap_2 and heap_4 */
#define configAPPLICATION_ALLOCATED_HEAP        0

/* Hook function related definitions. */
#define configUSE_IDLE_HOOK                     0
#define configUSE_TICK_HOOK                     0
#define configCHECK_FOR_STACK_OVERFLOW          0
#define configUSE_MALLOC_FAILED_HOOK            0
#define configUSE_DAEMON_TASK_STARTUP_HOOK      0

/* Run time and task stats gathering related definitions. */
#define configGENERATE_RUN_TIME_STATS           0
#define configUSE_TRACE_FACILITY                0
#define configUSE_STATS_FORMATTING_FUNCTIONS    0

/* Co-routine related definitions. */
#define configUSE_CO_ROUTINES                   0
#define configMAX_CO_ROUTINE_PRIORITIES         1

/* Software timer related definitions. */
#define configUSE_TIMERS                        1
#define configTIMER_TASK_PRIORITY               2
#define configTIMER_QUEUE_LENGTH                10
#define configTIMER_TASK_STACK_DEPTH            ( configMINIMAL_STACK_SIZE * 4 )

/* Interrupt nesting behavior configuration. */

/* Kernel interrupts are the SysTick and PendSV */
#define configKERNEL_INTERRUPT_PRIORITY         0xF	/* CM4 uses 4 bit priority */

/* Sets the highest priority of interrupts 
 * that can be blocked by a critical section
 */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY    5
#define configMAX_API_CALL_INTERRUPT_PRIORITY   configMAX_SYSCALL_INTERRUPT_PRIORITY


/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */
#define INCLUDE_vTaskPrioritySet                1
#define INCLUDE_uxTaskPriorityGet               1
#define INCLUDE_vTaskDelete                     1
#define INCLUDE_vTaskSuspend                    1
#define INCLUDE_xResumeFromISR                  0
#define INCLUDE_vTaskDelayUntil                 1
#define INCLUDE_vTaskDelay                      1
#define INCLUDE_xTaskGetSchedulerState          1
#define INCLUDE_xTaskGetCurrentTaskHandle       1
#define INCLUDE_uxTaskGetStackHighWaterMark     0
#define INCLUDE_eTaskGetState                   0
#define INCLUDE_xTaskGetIdleTaskHandle          1
#define INCLUDE_pxTaskGetStackStart             1
#define INCLUDE_xEventGroupSetBitFromISR        0
#define INCLUDE_xTimerPendFunctionCall          0
#define INCLUDE_xTaskAbortDelay                 0
#define INCLUDE_xTaskGetHandle                  0
#define INCLUDE_xTaskResumeFromISR              0

/* RTOS assert. Provide your own implementation */
#define configASSERT( x )

/* Definitions that map the FreeRTOS port interrupt handlers to their CMSIS
standard names - or at least those used in the unmodified vector table. */
#define vPortSVCHandler     SVC_Handler
#define xPortPendSVHandler  PendSV_Handler
#define xPortSysTickHandler SysTick_Handler

/* Dynamic Memory Allocation Schemes */
#define HEAP_ALLOCATION_TYPE1                   (1)     /* heap_1.c*/
#define HEAP_ALLOCATION_TYPE2                   (2)     /* heap_2.c*/
#define HEAP_ALLOCATION_TYPE3                   (3)     /* heap_3.c*/
#define HEAP_ALLOCATION_TYPE4                   (4)     /* heap_4.c*/
#define HEAP_ALLOCATION_TYPE5                   (5)     /* heap_5.c*/
#define NO_HEAP_ALLOCATION                      (0)

#define configHEAP_ALLOCATION_SCHEME            (HEAP_ALLOCATION_TYPE1)

#define configUSE_TICKLESS_IDLE                 0

#define configUSE_NEWLIB_REENTRANT              0

#endif /* FREERTOS_CONFIG_H */

