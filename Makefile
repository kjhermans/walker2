all:
	cc -g -O3 -Wall -Wextra -o pl *.c fw/*.c -lX11 -lXext
#	cc -g -O3 -Wall -Wextra -o pl *.c fw/*.c -lX11 -lXext -lbtree

clean:
	rm -f ./pl

archive: clean
	RELEASE=$$(cat release); \
	/bin/echo "  [TAR] ~/walker2-src-$$RELEASE.tar.gz"; \
	cd .. && \
	  tar czf ~/walker2-src-$$RELEASE.tar.gz \
	  --exclude=\.git walker2/
