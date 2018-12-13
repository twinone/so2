# so2

Linux-like operating system.

Built for [Operating Systems 2](https://www.fib.upc.edu/en/studies/bachelors-degrees/bachelor-degree-informatics-engineering/curriculum/syllabus/SO2) class at UPC.

**This course is by far the most fun of all I've done at UPC *(and I'm doing Computation)*.**
It's not the easiest but it's amazing to see your own operating system come together little by little,
and create a "functional" OS starting from something that just executes a C function.

### Features (what we implemented)
* interrupts
  * keyboard
  * clock
  * software interrupts
  * syscalls
* fast syscalls (sysenter/sysexit)
* multi-processing
  * task switch
  * fork
  * clone
* synchronization
  * semaphores (init/wait/signal/destroy)
  * yield
* dynamic memory management
  * sbrk
 
 
 ### Copying instructions

* Every year there are different restrictions to the project. Ours were:
  * No inline assembly code
  * Syscalls have to be implemented with the sysenter/sysexit instruction pair (no interrupts)

*It's ok to get some inspiration from this repository (you can even tell your teacher),
but don't just copy paste or you'll have a very hard time at the exam.*


### LICENSE

**The changes we made to the operating system base** are released hereby to the public domain,
which means you can do anything you like with it, including selling it, or claiming it as your work.

---


Thanks to [Juan José Costa](https://futur.upc.edu/JuanJoseCostaPrats) for making this course really amazing and for his patience with my bad jokes in class.













