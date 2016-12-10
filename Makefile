LIBYAML_REPO ?= https://github.com/yaml/libyaml
LIBYAML_BRANCH ?= master
DOCKER_NAME ?= libyaml-emitter
DOCKER_USER ?= $(USER)
DOCKER_TAG ?= latest
DOCKER_IMAGE ?= $(DOCKER_USER)/$(DOCKER_NAME):$(DOCKER_TAG)

define HELP
This Makefile supports the following targets:

    build    - Build ./libyaml-test-emitter
    test     - Run tests
    docker   - Build Docker image

endef
export HELP

help:
	@echo "$$HELP"

build: libyaml-emitter

libyaml-emitter: libyaml/tests/.libs/run-emitter
	cp $< $@

libyaml/tests/.libs/run-emitter: libyaml/tests/run-emitter.c libyaml/Makefile
	make -C libyaml

libyaml/tests/run-emitter.c: libyaml-emitter.c libyaml
	cp $< $@

libyaml/Makefile: libyaml
	( cd $< && ./bootstrap && ./configure )
	touch $@

libyaml:
	git clone $(LIBYAML_REPO) $@
	rm libyaml/tests/run-emitter.c

.PHONY: test
test: build
	prove -lv test/

docker:
	docker build --tag $(DOCKER_IMAGE) .

push: docker
	docker push $(DOCKER_IMAGE)

shell: docker
	docker run -it --entrypoint=/bin/sh $(DOCKER_IMAGE)

clean:
	rm -fr libyaml libyaml-emitter
