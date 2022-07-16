#
# docker factory
#
.PHONY = build-x64 tun clean
SRC = ../eko

build-x64: clean
	@git clone https://github.com/Software-Knife-and-Tool/eko.git
	@docker build -t gxx-12 -f Dockerfile.build .
	@docker run --rm -v /home/putnamjm/docker-factory/eko:/src -w /src gxx-12 make -C /src world
	@make -C eko tests > /dev/null 2>&1
	@make -C eko dist
	@docker build -t ubuntu-eko -f Dockerfile.ubuntu .

run:
	docker run -it ubuntu-eko /opt/eko/bin/eko

clean:
	@rm -rf eko
