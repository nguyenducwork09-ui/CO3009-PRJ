/**
 * @file    scheduler.c
 * @brief
 * @details
 */

#include "scheduler.h"
#include "main.h"
#include <string.h>

static sTask SCH_tasks_G[SCH_MAX_TASKS];

static uint8_t SCH_count = 0U;

static uint32_t next_task_id = 0U;

static volatile uint32_t sch_ticks = 0U;

static inline uint32_t sch_lock(void) {
  uint32_t primask = __get_PRIMASK();
  __disable_irq();
  return primask;
}

static inline void sch_unlock(uint32_t primask) { __set_PRIMASK(primask); }
static uint32_t get_new_task_id(void) {
  next_task_id++;
  if (next_task_id == NO_TASK_ID) {
    next_task_id++;
  }
  return next_task_id;
}

static uint32_t insert_task_locked(void (*pFunction)(void), uint32_t delay,
                                   uint32_t period, uint32_t task_id) {
  if (pFunction == (void *)0 || SCH_count >= SCH_MAX_TASKS) {
    return NO_TASK_ID;
  }

  uint8_t idx = 0U;
  uint32_t remaining = delay;
  while (idx < SCH_count && remaining >= SCH_tasks_G[idx].Delay) {
    remaining -= SCH_tasks_G[idx].Delay;
    idx++;
  }
  for (uint8_t i = SCH_count; i > idx; i--) {
    SCH_tasks_G[i] = SCH_tasks_G[i - 1U];
  }

  SCH_tasks_G[idx].pTask = pFunction;
  SCH_tasks_G[idx].Delay = remaining;
  SCH_tasks_G[idx].Period = period;
  SCH_tasks_G[idx].TaskID = task_id;
  SCH_tasks_G[idx].RunMe = (idx == 0U && remaining == 0U) ? 1U : 0U;
  if (idx < SCH_count) {
    SCH_tasks_G[idx + 1U].Delay -= remaining;
    SCH_tasks_G[idx + 1U].RunMe = 0U;
  }

  SCH_count++;
  return task_id;
}

void SCH_Init(void) {
  uint32_t primask = sch_lock();

  memset(SCH_tasks_G, 0, sizeof(SCH_tasks_G));
  SCH_count = 0U;
  next_task_id = 0U;
  sch_ticks = 0U;

  sch_unlock(primask);
}

uint32_t SCH_Add_Task(void (*pFunction)(void), uint32_t DELAY,
                      uint32_t PERIOD) {
  if (pFunction == (void *)0) {
    return NO_TASK_ID;
  }

  uint32_t primask = sch_lock();

  if (SCH_count >= SCH_MAX_TASKS) {
    sch_unlock(primask);
    return NO_TASK_ID;
  }

  uint32_t id = get_new_task_id();
  uint32_t result = insert_task_locked(pFunction, DELAY, PERIOD, id);

  sch_unlock(primask);
  return result;
}

uint8_t SCH_Delete_Task(uint32_t taskID) {
  if (taskID == NO_TASK_ID) {
    return 0U;
  }

  uint8_t found = 0U;
  uint32_t primask = sch_lock();

  for (uint8_t i = 0U; i < SCH_count; i++) {
    if (SCH_tasks_G[i].TaskID == taskID) {
      if (i + 1U < SCH_count) {
        SCH_tasks_G[i + 1U].Delay += SCH_tasks_G[i].Delay;
        SCH_tasks_G[i + 1U].RunMe =
            (i == 0U && SCH_tasks_G[i + 1U].Delay == 0U) ? 1U : 0U;
      }
      for (uint8_t j = i; j + 1U < SCH_count; j++) {
        SCH_tasks_G[j] = SCH_tasks_G[j + 1U];
      }

      SCH_count--;
      memset(&SCH_tasks_G[SCH_count], 0, sizeof(sTask));
      found = 1U;
      break;
    }
  }

  sch_unlock(primask);
  return found;
}

void SCH_Update(void) {
  sch_ticks++;

  if (SCH_count == 0U) {
    return;
  }

  if (SCH_tasks_G[0U].Delay > 0U) {
    SCH_tasks_G[0U].Delay--;
  }

  if (SCH_tasks_G[0U].Delay == 0U) {
    SCH_tasks_G[0U].RunMe = 1U;
  }
}

void SCH_Dispatch_Tasks(void) {
  while (1) {
    uint32_t primask = sch_lock();

    if (SCH_count == 0U || SCH_tasks_G[0U].RunMe == 0U) {
      sch_unlock(primask);
      break;
    }

    sTask current = SCH_tasks_G[0U];

    for (uint8_t i = 0U; i + 1U < SCH_count; i++) {
      SCH_tasks_G[i] = SCH_tasks_G[i + 1U];
    }
    SCH_count--;
    memset(&SCH_tasks_G[SCH_count], 0, sizeof(sTask));

    if (SCH_count > 0U) {
      SCH_tasks_G[0U].Delay += current.Delay;
      SCH_tasks_G[0U].RunMe = (SCH_tasks_G[0U].Delay == 0U) ? 1U : 0U;
    }

    if (current.Period > 0U) {
      insert_task_locked(current.pTask, current.Period, current.Period,
                         current.TaskID);
    }

    sch_unlock(primask);

    if (current.pTask != (void *)0) {
      current.pTask();
    }
  }
}

uint32_t SCH_Get_Time(void) {
  uint32_t primask = sch_lock();
  uint32_t ticks = sch_ticks;
  sch_unlock(primask);

  return ticks * SCH_TICK_MS;
}
