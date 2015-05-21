toxfile
=======

This repository contains several tools related to toxfiles.

`toxfile` is a general purpose utility for creating, modifying and inspecting
toxfiles (also called [tox] savedata or save files). These files contain data
which can be loaded by [tox] to restore a specific state, including user profile
information (name, status message, etc.) and the user's friends list.

A few examples:

`toxfile file.tox` will output user profile information stored in `file.tox`,
prompting for a password if the file is encrypted. The user's secret key will not
be output.

`toxfile -x file.tox` will output the user's secret key.

`toxfile -N ~/me.tox` will create a new tox file at `~/me.tox`.

`toxfile -e file.tox` will encrypt an existing toxfile `file.tox` with a password
using [libtoxencryptsave].

`toxfile -d file.tox` will decrypt the encrypted toxfile `file.tox`.

### Building

To build, just `make`

To build and run tests, `make check`

To build without libtoxencryptsave support, `make TOXFILE_NO_ENC=true`

[tox]:https://tox.im
[libtoxencryptsave]:https://github.com/irungentoo/toxcore/tree/master/toxencryptsave
