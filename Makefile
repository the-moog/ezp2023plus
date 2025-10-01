

SHELL = /bin/bash -vc

MESON := meson
BUILD_DIR := buildDir

# Returns name if exists else nothing
test_for_dir = $(shell test -d $1 && echo $1)
test_for_file = $(shell test -f $1 && echo $1)

.PHONY: Makefile

all: appImage

.PHONY: prep
prep: $(if $(call test_for_dir,${BUILD_DIR}),clean)
	${MESON} setup ${BUILD_DIR} .

${BUILD_DIR}: prep

.PHONY: build
build: ${BUILD_DIR}
	meson compile -C buildDir

.PHONY: clean
clean:
	${MESON} setup --wipe ${BUILD_DIR}
	rm -rf build ${BUILD_DIR}

.PHONY: appImage
appImage: clean prep
	./dist.sh

