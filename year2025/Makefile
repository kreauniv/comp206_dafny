# Minimal makefile for Sphinx documentation
#

# You can set these variables from the command line, and also
# from the environment for the first two.
SPHINXOPTS    ?=
SPHINXBUILD   ?= sphinx-build
SOURCEDIR     = source
BUILDDIR      = build

# Put it first so that "make" without argument is like "make help".
help:
	@$(SPHINXBUILD) -M help "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)

.PHONY: help Makefile

# Catch-all target: route all unknown targets to Sphinx using the new
# "make mode" option.  $(O) is meant as a shortcut for $(SPHINXOPTS).
%: Makefile
	@$(SPHINXBUILD) -M $@ "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)

pages: html
	cd build/html && tar zcf /tmp/comp206-html.tar.gz . 
	cd /tmp && git clone git@github.com:kreauniv/comp206.git comp206_build
	cd /tmp/comp206_build \
		&& git checkout gh-pages \
		&& tar zxf /tmp/comp206-html.tar.gz \
		&& git add . \
		&& git commit -m "Updated gh-pages" \
		&& git push
	rm -rf /tmp/comp206_build
	rm /tmp/comp206-html.tar.gz

