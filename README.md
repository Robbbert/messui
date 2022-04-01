
# **MESSUI** #


What is MESSUI?
===============

MESSUI is an easy-to-use frontend of MESS, for Windows 7 and later.


License
=======

* The license of MAME is explained at the MAME repository ( https://github.com/mamedev/mame ).
* The license of WINUI (the graphical frontend) is BSD3Clause.


How to compile?
=============

To create the command-line build:

```
make subtarget=mess OSD=newui
```

To create the graphical frontend build:

```
make subtarget=mess OSD=messui
```

For MESSUI, you must use GCC 10.1 - later versions will compile but crash at start.


Where can I find out more?
=============

* [Official MAME Development Team Site](http://mamedev.org/) (includes binary downloads for MAME and MESS, wiki, forums, and more)
* [Official MESS Wiki](http://mess.redump.net/)
* [MAME Testers](http://mametesters.org/) (official bug tracker for MAME and MESS)
* [Official MESSUI site](http://messui.1emulation.com/)
* [Official MESSUI forum](http://1emulation.com/pc/messui) (bugs, requests, discussion)


Licensing Information
=====================

Information about the MAME content can be found at https://github.com/mamedev/mame/blob/master/README.md

Information about the MAME license can be found at https://github.com/mamedev/mame/blob/master/COPYING

Information about the WINUI portion can be found at https://github.com/Robbbert/mameui/blob/master/docs/winui_license.txt

