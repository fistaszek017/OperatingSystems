CC = gcc
CFLAGS = -Wall
SRCS = sig_default.c sig_mask.c sig_ignore.c sig_handle.c
OBJS = $(SRCS:.c=.o)


all: libsig.a libsig.so child_static child_shared child_dynamic main


libsig.a: $(OBJS)
	ar rcs $@ $^


libsig.so: $(SRCS)
	$(CC) $(CFLAGS) -fPIC -shared -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@


child_static: child.c libsig.a
	$(CC) $(CFLAGS) -o $@ $< -L. -l:libsig.a


child_shared: child.c libsig.so
	$(CC) $(CFLAGS) -o $@ $< -L. -lsig


child_dynamic: child.c libsig.so
	$(CC) $(CFLAGS) -DDYNAMIC -o $@ $< -L. -lsig -ldl

# --- main (jeden dla wszystkich wersji) ---
main: main.c
	$(CC) $(CFLAGS) -o main main.c

# --- sprzątanie ---
clean:
	rm -f *.o *.a *.so child_static child_shared child_dynamic main