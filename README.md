# ElevatorKernelModule

# Project Objectives
Gain hands-on experience with system calls: Understand the fundamental concepts and implementation details of system calls by working on various aspects of kernel programming.
Explore kernel programming: Develop a deeper understanding of kernel programming by delving into the intricacies of concurrency and synchronization mechanisms within the kernel.
Enhance knowledge of concurrency and synchronization: Apply the acquired knowledge of concurrency and synchronization in the kernel environment to effectively manage and control concurrent access to shared resources.
Explore different techniques and approaches to ensure thread safety and prevent race conditions.

# Project Description 
This project aims to provide a comprehensive understanding of system calls, kernel programming, concurrency, synchronization, and elevator scheduling algorithms.

By completing this project, you acquire practical experience in system calls, kernel programming, concurrency, synchronization, and scheduling algorithms. These are essential skills for developing efficient and robust software systems, particularly in operating systems and low-level programming domains. Understanding system calls and kernel programming enables you to interact with and extend the functionality of the operating system, while concurrency, synchronization, and scheduling concepts are crucial for efficient resource management and multitasking in complex systems

# Part 1 - System-Call Tracing 
In Part 1, you start by working with system calls. By adding system calls to a C program and verifying their correctness using the "strace" tool, you gain hands-on experience with system call integration and learn about the available system calls for your machine. This part lays the foundation for understanding how system calls interact with the kernel

# Part 2 - Timer Kernel Module
Part 2 takes you further into kernel programming. You will develop a kernel module called "my_timer" that retrieves and stores the current time using the "ktime_get_real_ts64()" function. This module creates a proc entry and allows you to read the current time and elapsed time since the last call. This part helps you understand how kernel modules work, how to interact with kernel functions, and how to use proc interfaces for communication.

# Part 3 - Elevator Module
Part 3 focuses on a more complex task: implementing a scheduling algorithm for a dorm elevator. You create a kernel module representing the elevator, supporting operations like starting, stopping, and issuing requests. The module also provides a "/proc/elevator" entry to display important elevator information. This part challenges you to manage concurrency, synchronization, and efficient scheduling within the kernel environment.


# division of labor (after) 
Project 2: Division of Labor
Operating Systems
Team Members:
1. Gabriela Pedreros
2. Tori Lee
3. Jacob Sonn

Part 1: System Call Tracing
- Gabriela Pedreros
Part 2: Timer Kernel Module
- Jacob Sonn
- Gabriela Pedreros
Part 3a: Adding System Calls
- Tori Lee
Part 3b: Kernel Compilation
- Tori Lee
Part 3c: Threads
- Gabriela Pedreros
Part 3d: Linked List
- Jacob Sonn
Part 3e: Mutexes
- Tori Lee
- Gabriela Pedreros
Part 3f: Scheduling Algorithm
- Jacob Sonn


# file listing
part 1/
 empty.c
 empty.trace
 part1.c
 part1.trace
 Makefile
part 2/
 src/
 Makefile
part 3/
 src/
 Makefile
 syscalls.c
Makefile
README.md

*For this project, you only need to organize your source code and header files into the src/ folder.

Do not include your final executable(s) in your repository, doing so will result in a deduction!

For part 2 and part 3, your makefile should produce a kernel module, my_timer.ko and elevator.ko respectively.

# description of how to compile and run program 
