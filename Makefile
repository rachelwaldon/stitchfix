COMPONENT_NAME := Warehouse Customers DistributionCenter Repeater
COMPONENT_LIB := libStitchFix.so

SRC_FILES := $(addsuffix .cpp, $(COMPONENT_NAME))
OBJS := $(patsubst %.cpp, %.o, $(SRC_FILES))
CPPFLAGS := $(shell pkg-config SST-12.0.1 --cflags)
CXXFLAGS := -std=c++17 -g -fPIC -I/opt/sst/sst-core/sst-core-install/include/sst/core
LDFLAGS := $(shell sst-config --ELEMENT_LDFLAGS)

all: $(COMPONENT_LIB) install

$(COMPONENT_LIB): $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@

install:
	sst-register StitchFix StitchFix_LIBDIR=$(CURDIR)	

clean:
	rm *.o $(COMPONENT_LIB)
	sst-register -u StitchFix
