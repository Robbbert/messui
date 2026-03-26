
# **MESSUI** #


What is MESSUI?
===============

MESSUI is an easy-to-use frontend of MESS, for Windows 7 SP 1 and later.



How to compile?
=============

To create the command-line build:

```
make subtarget=mess OSD=newui PTR64=1 SYMBOLS=0 NO_SYMBOLS=1 DEPRECATED=0
```

To create the graphical frontend build:

```
make subtarget=mess OSD=messui PTR64=1 SYMBOLS=0 NO_SYMBOLS=1 DEPRECATED=0
```


Where can I find out more?
=============

* [Official MAME Development Team Site](http://mamedev.org/)
* [MAME Testers](http://mametesters.org/) (official bug tracker)
* [Official MESSUI site](http://messui.1emulation.com/)
* [Official MESSUI forum](http://1emulation.com/forums/forum/125-mameui) (bugs, requests, discussion)


Licensing Information
=====================

The primary license is GPL_2.0 : https://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html

Information about the MAME content can be found at https://github.com/mamedev/mame/blob/master/README.md

Information about the MAME license can be found at https://github.com/mamedev/mame/blob/master/COPYING

Information about the WINUI portion can be found at https://github.com/Robbbert/mameui/blob/master/docs/winui_license.txt

