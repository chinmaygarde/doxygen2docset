build preset='default':
	@cmake --build --preset {{preset}}

alias gen := setup

setup preset='default':
	cmake --preset {{preset}}

clean:
	rm -rf build

sync:
	git submodule update --init --recursive -j 8

list-builds:
	@cmake --list-presets
