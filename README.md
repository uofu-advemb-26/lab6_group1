# Activity 0
Semaphore Priority Inversion Effects: 
![Semaphore Priority Inversion Effects](./SemaphoreInversionEffects.png "Semaphore Priority Inversion Effects")
  - The low priority task takes the semaphore; the high priority task waits on the low priority task to release the semaphore; the medium priority task starves the low priority task and prevents it from releasing the semaphore. The result: unbounded priority inversion.