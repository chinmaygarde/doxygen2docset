build preset='default':
	@cmake --build --preset {{preset}}

alias gen := setup

setup preset='default':
	cmake --preset {{preset}}

test preset='default':
	@ctest --preset {{preset}}

clean:
	rm -rf build

sync:
	git submodule update --init --recursive -j 8

list-builds:
	@cmake --list-presets

ci-build-package:
	just sync
	just gen release
	just build release
	just test release
	rm -rf build/release/installed
	rm -rf build/to_upload
	mkdir build/to_upload
	cmake --install build/release --prefix=build/release/installed
	tar -czvf build/to_upload/doxygen2docset-{{ os() }}-{{ arch() }}.tar.gz -C build/release/installed .
