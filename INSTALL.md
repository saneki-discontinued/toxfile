Installation
============

# Dependencies


| Name      | Version  |
|-----------|----------|
| glibc     | >=2.13   |
| [Jansson] | >=2.4    |
| [toxcore] | latest   |
| [libsy]   | latest   |



`toxfile` and `toxdump` both depend on [toxcore] and the tiny C project [libsy], which is currently included as a submodule.

To get [libsy]:
```
$ git submodule update --init
```

`toxdump` also depends on `Jansson`.
Although `toxdump` supports loading `Jansson` dynamically, the header file is expected to be installed when building (jansson.h).


# Building


After the dependencies are in place, building should require no more than:
```
$ make
```


### Environment variables changing behaviour of build:

| Variable | Change |
|----------|--------|
| `TOXFILE_NO_ENC=true` | disable encryption/decryption support [1] |
| `TOXDUMP_SO_JANSSON=true` | link statically against `libjansson` [2] |


`[1]` - may be useful if libtoxencryptsave is not available for whatever reason.</br>
`[2]` - will force `toxdump` to be linked against libjansson, instead of trying to load it dynamically via libdl.


[toxcore]:https://github.com/irungentoo/toxcore
[libsy]:https://github.com/saneki/libsy
[Jansson]:http://www.digip.org/jansson/