
OneWire library for Arduino with MAX31850 support
-------------------------------------------------

=================================================================
This version is based on V2.0 from pjrc but with MAX31850 support
=================================================================

Version 2.0 fixes search bugs (thanks to Robin James) and interrupt issues.
http://www.pjrc.com/teensy/td_libs_OneWire.html


This is a slightly modified version of the OneWire library originally written by Jim Studt for arduino-0007 and later updated for arduino-0008 by Josh Larios.  This new version eliminates the large lookup table that was previously used by the checksum calculation - otherwise it's identical to Josh's version.  It was developed under arduino-0010.

For a general description and links, see

    http://www.arduino.cc/playground/Learning/OneWire

Jim Studt's original verion of this library is still available at

    http://www.federated.com/~jim/onewire/

Josh Larios' version is available at 

    http://www.elsewhere.org/onewire/

To install this library, you should just have to unzip the archive in the arduino-0010/hardware/libraries directory.  It will create a subdirectory
called 'OneWire'.

Tom Pollard
pollard@alum.mit.edu
May 20, 2008
