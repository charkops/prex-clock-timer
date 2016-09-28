# prex-clock-timer

Author : Kopsacheilis Charalampos

Date   : 28 / 9 / 2016

This is a Real time clock-timer implemented on the Prex RTOS.
This is a project for class "Embedded and Real time systems" at the Aristotele's University of Thessaloniki (AUTH), Thessaloniki - Greece

In order for this to run, we must configure Prex to run the source code as a single Real Time task (RT) at startup and nothing else. Instructions on how to compile up prex are at the end of this file.



Basic functions of our programm : 

At startup only the time will be shown at the standard output in format :   HH:MM:SS . 
This is called realTime mode.
There is also a stopWatch mode, which displays the time passed since stopWatch was activated.

By pressing 't' - toggle we can switch back and forth between realTime and stopWatch mode.

#REALTIME : 

By pressing 'h' in realTime mode we can increase the hour by +1.

By pressing 'm' in realTime mode we can increase the minutes by +1.

By pressing 'z' in realTime mode we can zero out the seconds.

#STOPWATCH :

By pressing 'r' in stopWatch mode we can reset the stopWatch.

By pressing 's' in stopWatch mode we can (re-)start and stop the stopWatch.

By pressing 'p' in stopWatch mode we can pause the stopWatch.


