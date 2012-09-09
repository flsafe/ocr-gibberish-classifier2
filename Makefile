CFLAGS=-g -O2 -Wall -Wextra -Isrc -rdynamic -DNDEBUG $(OPTFLAGS)
LDLIBS=-ldl $(OPTLIBS)
PREFIX?=/usr/local

SOURCES=$(filter-out src/main.c, $(wildcard src/**/*.c src/*.c))
OBJECTS=$(patsubst %.c, %.o, $(SOURCES))

EXECSOURCES=$(wildcard src/**/*.c src/*.c)
EXECOBJECTS=$(patsubst %.c, %.o, $(EXECSOURCES))

TEST_SRC=$(wildcard tests/*_tests.c)
TESTS=$(patsubst %.c,%,$(TEST_SRC))

TARGET=build/libYOUR_LIB.a
SO_TARGET=$(patsubst %.a, %.so, $(TARGET))

all: $(TARGET) $(SO_TARGET) exec tests

exec: $(EXECOBJECTS) 
	$(CC) -o bin/$@ $(EXECOBJECTS)

dev: CFLAGS=-g -Isrc -Wall -Wextra $(OPTFLAGS)
dev: all

$(TARGET): CFLAGS += -fPIC
$(TARGET): build $(OBJECTS)
	ar rcs $@ $(OBJECTS)
	ranlib $@

$(SO_TARGET): $(TARGET) $(OBJECTS)
	$(CC) -shared -o $@ $(OBJECTS)

build:
	@mkdir -p build
	@mkdir -p bin

.PHONY: tests
tests: CFLAGS += $(TARGET)
tests: $(TESTS)
	sh ./tests/runtests.sh

valgrind:
	VALGRIND="valgrind --log-file=/tmp/valgrind-%p.log" $(MAKE)

clean:
	rm -rf build $(OBJECTS) $(TESTS)
	rm -rf bin $(EXECOBJECTS)
	rm -rf tests/tests.log
	find . -name "*.gc" -exec rm {} \;
	rm -rf `find . -name "*.dSYM" -print`

install: all
	install -d $(DESTDIR)/$(PREFIX)/lib/
	install $(TARGET) $(DESTDIR)/$(PREFIX)/lib/

BADFUNCS='[^_.>a-zA-Z0-9](str(n?cpy|n?cat|xfrm|n?dup|str|pbrk|tok|_)|stpn?cpy|a?sn?printf|byte_)'
check:
	@echo Files with potentially dangerous functions
	@egrep $(BADFUNCS) $(SOURCES) || true
