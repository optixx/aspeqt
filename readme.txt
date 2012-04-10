AspeQt : Atari Serial Peripheral Emulator for Qt

= Summary =

AspeQt emulates Atari SIO peripherals when connected to an Atari 8-bit computer with an SIO2PC cable. In that, respect it's similar to programs like APE and Atari810. The main difference is that it's free (unlike APE) and it's cross-platform (unlike Atari810 and APE).

Some features:

* Qt based GUI with drag and drop support.
* Cross-platform (currently Windows and x86-Linux, but it should be easy to port it to MacOsX and othe Unix flavors)
* 8 disk drive emulation
* Text-only printer emulation with saving and printing of the output
* Cassette image playback
* Folders can be mounted as simulated Dos20s disks. (read-only, at least for now)
* Atari executables can be booted directly, optionally with high speed.
* Contents of image files can be viewed / changed
* AspeQt Client module (ASPECL.COM). Runs on the Atari and is used to get/set Date/Time on the Atari, more functionality will be added in future releases
* Support for APE time downloader (deprecated as of March 26, 2012, use ASPECL.COM instead)
* Upto 6x SIO speed and more if the serial port adaptor supports it.
* Localization support

= Credits =

Many thanks to:
    Ray Ataergin for Turkish translation, PC print support, ATASCII font support, ASPECL and other improvements.
    Krzysztof Pyrkosz for adding language selection, polish translation and other fixes and improvements.
    Matthias Reichl for creating AtariSIO package, letting me use his high speed SIO codes and even getting his hands dirty with the Unix serial port backend.
    Sebastian Bartkowicz for kindly donating one of his Candle'O'Sin I/O boards, thus giving me the possibility to support it and other FTDI chip based USB adaptors.
    Nikita Zimin for Russian translation.

= License =

AspeQt: Copyright 2009 by Fatih Aygün. You can freely copy, use, modify and distribute it under the GPL 2.0 license. Please see license.txt for details.

Qt libraries: Copyright 2009 Nokia Corporation and/or its subsidiary(-ies). Used in this package under LGPL 2.0 license.

Silk Icons: Copyright by Mark James (famfamfam.com). Used in this package under Creative Commons Attribution 3.0 license.

Additional Icons by Oxygen Team. Used in this package under Creative Commons Attribution-ShareAlike 3.0 license.

AtariSIO Linux kernel module and high speed code used in the EXE loader: Copyright Matthias Reichl <hias@horus.com>. Used in this package under GPL 2.0 license.

Atascii Fonts by Mark Simonson (http://members.bitstream.net/~marksim/atarimac). Used in this package under Freeware License.


= Contact =

Just drop a line at cyco130@yahoo.com
Please include the word "aspeqt" in the subject field.

For SIO2PC and 10502PC hardware purchase/support visit www.atari8warez.com, for SIO2PC/10502PC and AspeQt related issues please contact Ray Ataergin at ray@atari8warez.com.
