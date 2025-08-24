.PHONY: all clean sysmodule

all: sysmodule

sysmodule:
	@echo "Building XMusic sysmodule..."
	@./build.sh

clean:
	@echo "Cleaning build files..."
	@rm -rf sysmodule/build sysmodule/*.elf sysmodule/*.nso sysmodule/*.map
	@rm -rf dist
