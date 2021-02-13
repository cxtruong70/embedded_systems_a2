OUTFILE = array_sorter_test
OUTFILE_HOST = array_sorter_host
OUTDIR_HOST = $(HOME)/cmpt433/work/as2
OUTDIR = $(HOME)/cmpt433/public/myApps
CFILE = main.c sorter.c socket.c

# setup output output once, used twice

HOST_COMPILE = gcc
CROSS_COMPILE = arm-linux-gnueabihf-
CC_C = $(CROSS_COMPILE)gcc
CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror -pthread

# list and run these other targets before running all
all: app done

help:
	@echo "Build assignment 1 program for BeagleBone."
	@echo "Targets include all, app, host-app, nestedDir, and clean."

app:
	$(CC_C) $(CFLAGS) $(CFILE) -o $(OUTDIR)/$(OUTFILE)
	ls -l $(OUTDIR)/$(OUTFILE)

host-app:
	$(HOST_COMPILE) $(CFLAGS) $(CFILE) -o $(OUTDIR_HOST)/$(OUTFILE_HOST)
	ls -l $(OUTDIR_HOST)/$(OUTFILE_HOST)

nestedDir:
	make --directory=myNestedFolder

done:
	@echo "*** Finished building and deploying application"

clean: 
	rm $(OUTDIR)/$(OUTFILE)
