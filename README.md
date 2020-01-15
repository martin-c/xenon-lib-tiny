# xenon-lib-tiny: Task Scheduler and HAL for the tinyAVR 1-series

# Description

This library contains tools for quickly developing efficient simple to moderately complex applications 
for the tinyAVR 1-series microcontrollers.
The library is divided into two major components:
* The task scheduler and components related to synchronization/communication, and
* the HAL (Hardware Abstraction Layer).

The goal of the HAL is to wrap the major tinyAVR peripherals in programmer-friendly data structures and functions.
The goal of the task scheduler is to enable quickly building applications with multiple cooperatively-scheduled 
real-time tasks.

Currently the library targets the tinyAVR 3216/1616 and 3217/1617 devices, although this may be expanded in the 
future as required.
