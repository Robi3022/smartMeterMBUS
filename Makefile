PROJECT_ROOT = $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

OBJS = Converter.o Encrypt.o


ifeq ($(BUILD_MODE),debug)
	CFLAGS += -g
else ifeq ($(BUILD_MODE),run)
	CFLAGS += -O2
else ifeq ($(BUILD_MODE),linuxtools)
	CFLAGS += -g -pg -fprofile-arcs -ftest-coverage
	LDFLAGS += -pg -fprofile-arcs -ftest-coverage
	EXTRA_CLEAN += Converter.gcda Converter.gcno $(PROJECT_ROOT)gmon.out
	EXTRA_CMDS = rm -rf Converter.gcda
else
    $(error Build mode $(BUILD_MODE) not supported by this Makefile)
endif

all:	Converter

Converter:	$(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^ -lmbedtls -lmbedx509 -lmbedcrypto
	$(EXTRA_CMDS)

%.o:	$(PROJECT_ROOT)%.cpp
	$(CXX) -c $(CFLAGS) $(CXXFLAGS) $(CPPFLAGS) -o $@ $<

%.o:	$(PROJECT_ROOT)%.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -o $@ $<

clean:
	rm -fr Converter $(OBJS) $(EXTRA_CLEAN)
