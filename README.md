Gvolicon
=========

**A simple and lightweight volume icon that sits in your system tray.**

Features
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

Some features are not yet fully implemented:

* Detect mute-change in `tray_icon_check_for_update`;
* Extend device to use;
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
**Gvolicon** is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

**Gvolicon** is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see <http://www.gnu.org/licenses/>.

**Copyright © 2013** Jente Hidskes <jthidskes@outlook.com>
