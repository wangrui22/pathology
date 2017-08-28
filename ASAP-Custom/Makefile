ver = debug

MAKE = make
subsystem:
	-mkdir bin
	-mkdir bin/formats
#	cd bin ; mkdir formats
	$(MAKE) -C ./Core
	$(MAKE) -C ./IO/MultiResolutionImageInterface
	$(MAKE) -C ./IO/OpenSlideFileFormat
	$(MAKE) -C ./Annotation
	$(MAKE) -C ./Executables/AnnoServer
	$(MAKE) -C ./Executables/DBImporter
#todo add debug/release

.PHONY : clean
clean:
	make clean -C ./Core
	make clean -C ./IO/MultiResolutionImageInterface
	make clean -C ./IO/OpenSlideFileFormat
	make clean -C ./Annotation
	make clean -C ./Executables/AnnoServer
	make clean -C ./Executables/DBImporter

