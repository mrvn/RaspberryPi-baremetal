A more complex build system
===========================

The build system is made to be compact and fast. There is a single
main Makefile of just 114 lines that contains all the logic to compile
stuff and a small helper (target.mk) to switch to the build directory.
Every directory further contains a Makefile.obj listing the sub
directories and object files and special flags, if any, used to
compile objects files for that directory. Entries can be conditional
allowing files or subdirectories to be skipped completely if not
wanted. The Makefile.obj are processed by scripts/gen-Makefile.mk to
produce dependencies and target local variables in Makefile syntax,
which the main Makefile then includes.

Features:
---------

1) out-of-tree build / per arch build directory

The generated files are stored in _&lt;arch triplet&gt; leaving the source
directly nice and clean.

2) non-recursive Makefile

The Makefile does not recursively invoke sub makes. Everything is done
in a single make instance using dependencies.

Note: there is one $(MAKE) call to switch to the build directory but that is
it.

3) automatic dependencies

During build the dependencies of assembler, C and C++ files are
recorded and used for the next build to determine what needs building
again.

4) builds temporary libraries for each subdirectory

When the number of source files exceeds a certain number the command
line length would exceed the allowed maximum. To avoid that all the
object files in a directory are linked together into a temporary
library (_built-lto/obj-y.o). If the directory has subdirectories then
the _built-lto/obj-y.o files of the subdirectories are linked in as well.

When a source file changes the automatic dependencies will cause it to
be recompiled, followed by the _built-lto/obj-y.o file for that directory
and then its parent up to the top directory.

5) sources can be conditional

To include a source file in a build the respective object file has to
be specified in Makefile.obj as: "OBJ y foo.o". A source file can also
be included conditionally using "OBJ $(CONFIG_BLA) blub.o". If
$(CONFIG_BLA) evaluates to y, or y was used directly, then the source
is compiled and the object file included. If it evaluates to anything
else then the file is ignored completely.

Note: spaces are not allowed in the conditional, apart from that
pretty much any make construct is valid.

6) allows special options for specific files

The ASFLAGS, CFLAGS or CXXFLAGS used to compile an object file can be
overridden for any object file using "ASFLAGS foo.o = option" or, more
likely useful, they can be extended using "ASFLAGS foo.o += option".

7) subdirectories can be conditional

Similar to files subdirectories need to be specified in Makefile.obj
using "DIR y subdir". And just like files they can be conditional,
e.g. "DIR $(CONFIG_FPU) fpu". If $(CONFIG_FPU) evaluates to y, or y
was used directly, then the _built-in-y.o in the directory is
included. Which means source files in that subdirectory are compiled
and the object files included in the subdir/_built-in-y.o first. If it
evaluates to n then the subdirectory is ignored completely, which also
means none of the source files there are compiled.

8) terse output during compile

The full command to compile a source file can get rather long with all
the options and full path for each file making things
unreadable. Therefore by default running just make will give terse
output listing just the action and name of the object file being
processed. Using "make VERBOSE=1" will print the full command instead.

9) allows partial use of link time optimization

Object files are compiled with -flto if they use a .lto extension. By
using foo.o or foo.lto link time optimization can be selected on a
file by file basis.

Further link time optimization can be limited to within a subdirectory
by specifying DOLTO as first line in Makefile.obj. This causes link
time optimization to be performed on all the objects files inside that
directory as well as the _tmp-y.o and _tmp-y.lto temporary libraries
from subdirectories. The result is put into _tmp-y.o and the
_tmp-y.lto remains empty.

10) allows remapping sections for directories

If a Makefile.obj for a directory starts with LDSCRIPT .<suffix> then
the .text, .rodata, .data and .bss sections are mapped to
.text.<suffix>, .rodata.<suffix>, .data.<suffix>, .bss.<suffix>
respectively. For this to work LDSCRIPT implies DOLTO.


ToDo:
-----

1) generate linker script so the remapped sections of each future
process are mapped independently in low memory (all having overlapping
VMA but unique LMA).
