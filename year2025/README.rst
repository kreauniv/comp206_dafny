Generating the notes
--------------------

On Unix-like systems ....


::

    make latexpdf

The above command will generate a PDF file
``build/latex/principlesofprogrammingiimperativecomputation.pdf``.  The symbolic
link named ``comp206-notes.pdf`` in the current directory points to this file, so
once built, you can open the symlink.

You can also run ::

    make html

... to generate a web page version of the notes, which you can view by opening
``build/html/index.html``. The generated html comes complete with a local search
facility to enable easy navigation.

The ``c0`` folder contains code snippets for the course. The ``screencasts``
folder contains the code worked out in screencasts shared with the students.

The current version of the compiled notes (PDF form) is available here -
`comp206-notes.pdf <https://drive.google.com/file/d/1pU2Q8ocYDqQq4ZZKClT8uz9HTKhfVSYw/view?usp=sharing>`_ .

Prerequisites
-------------

You need a TeX system installed, including the commands to run ``pdflatex`` and such.
You need to have the Sphinx system installed, which you can do using ::

    pip install -r requirements.txt

Links to recordings/screencasts
-------------------------------

1. `Stacks <https://drive.google.com/file/d/1SqkZ_KsXyKJ52CfAH3s9NLiznGgGoqkG/view?usp=sharing>`_
2. `Amortized complexity part 1 <https://drive.google.com/file/d/1ZrPB_f3fyKAw5KxdOoBKFYXKGoYNpqhS/view?usp=sharing>`_
3. `Amortized complexity part 2 <https://drive.google.com/file/d/1idm5IcL1gksJudIXn5iXRjV9ik39p0Fg/view?usp=sharing>`_
4. `Computational complexity <https://drive.google.com/file/d/1TRcIjOTfjOOUHUDGDg_xUza4VmVAE9T3/view?usp=sharing>`_
5. `Generic sorting interface <https://drive.google.com/file/d/1fjPdwpFnzkjcqE5xCIzT6otg8Fm4-XKr/view?usp=sharing>`_
6. `Generic stack <https://drive.google.com/file/d/19LE6F5EEEeC91_PIspsCH2iKy1MR8q8Z/view?usp=sharing>`_
7. `Unbounded arrays <https://drive.google.com/file/d/15Hf4M-2HyKJflNCm9Ka54yoHXGRH6kmO/view?usp=sharing>`_
8. `Walking through binary search <https://drive.google.com/file/d/1JYbdKGsNvfmUvhRDxmbp3VdbuW89Mn4V/view?usp=sharing>`_



