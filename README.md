# Cpu Usage Tracker
Console-based program for tracking logical processor usage across system.

## Components
Program is implemented through five synchronized threads communicating with each other.

### Threads
- `reader` - Extracts data from `/proc/stat` file and passing it futher down the line.
- `analyzer` - Takes raw data about time spent in various processor states and calculates usage statistics.
- `printer` - Displays usage statistics for every processor in console using percentage format.
- `logger` - Saves logging messages to text file for troubleshooting purposes.
- `watchdog` - Responsible for monitoring other threads for responsiveness and program termination in case of failure.
<!--
### Libraries
- `circbuf` - Simple circular buffer library following object-oriented design schemes.
-->