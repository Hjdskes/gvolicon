Gvolicon
=========

**A simple and lightweight volume icon that sits in your system tray, written in GTK3.**

Usage
--------

The below listed features should give you an idea of what Gvolicon is and what it is not. You can use these to see if Gvolicon is suited for your needs.

* Easily change volume levels by scrolling on the icon;
* Easily mute and restore the volume by clicking on the icon;
* No right-click menu.

Gvolicon's help output should also give you an idea of what it is meant to do:

	-v, --version              Display version and exit
	-d, --device               Device to use
	-m, --mixer                Mixer to use
	-u, --update-interval      Set update interval in seconds
	-s, --volume-step          Set volume step to increase or decrease the volume with
	-i, --symbolic-icons       Use symbolic icons

Installation
------------

The dependencies required to build and run Gvolicon are GTK3 and alsa.

Once these dependencies are installed, just run the following commands to build and install Gvolicon:

    $ make
    # make clean install

ToDo
----

Some features are not yet implemented:

* Implement choosing sound card?
* List all available
	* mixers;
	* devices;
	* sound cards?

Bugs
----

For any bug or request [fill an issue][bug] on [GitHub][ghp].

  [bug]: https://github.com/Unia/gvolicon/issues
  [ghp]: https://github.com/Unia/gvolicon

License
-------

This project is licensed under the GNU General Public License and should be treated as such. See `COPYING` for more information.

**Copyright © 2013-2014** Jente Hidskes <hjdskes@gmail.com>
