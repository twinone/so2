*** This tar file contains the following files:

* libjpsemaphores.a: Test suite for testing the semaphores code. It contains
  the user calls 'runjp' and 'runjp_rank' to execute the tests plus all the
  necessary code to execute them.
* libjpclone.a: Test suite for testing the clone code. It also contains the
  user calls 'runjp' and 'runjp_rank' to execute the tests plus all the
  necessary code to execute them.
* README_E3.txt: this file specifies the pre-requisites for each test.


*** INSTALLATION:

* You have to:
	1) Create a soft link named libjp.a pointing to the library with the
  tests that you want to run (for example: libjpsemaphores.a).
		$ ln -s libjpsemaphores.a libjp.a

	2) Modify the Makefile so the user target is linked with the libjp.a file.
		...
		USROBJ = libc.o  libjp.a
		...

	3) Modify the Makefile so the system target is linked with the libauxjp.a
  file as this test suite needs some special features provided there.
		...
		LIBZEOS= -L. -l zeos -l auxjp
		...

	4) Modify the main function of system.c, adding a new call to the routine 'zeos_init_auxjp()'
  just before enabling interrupts and changing to user mode to initialize these features.
		...
		zeos_init_auxjp();
		enable_int();
		...

	5) Modify the main function of the user.c file to call the 'runjp' routine.
		...
		main(void)
		{
		runjp();
		while(1);
		}

	6) Enable Bochs to write on the console, in order to read all the
  test messages without problems (.bochsrc).
		...
		port_e9_hack: enabled=1



*** NOTES:

* You have to take into account that the tests in a test suite are cumulative.
  That is, if one test fails then the execution of the rest of the tests may be
  inconsistent.

* You can execute just a range of tests, replacing  the call to runjp() in the
  main function by a call to runjp_rank(int first, int last), where "first" is
  the identifier of the first test in the range that you want to execute, and
  "last" is the last one. Tests that not fall inside the rank will be evaluated
  as SKIPPED.
