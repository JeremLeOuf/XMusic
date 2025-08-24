.PHONY: all clean sysmodule overlay

all: sysmodule overlay

sysmodule:
	@$(MAKE) -C sysmodule

overlay:
	@$(MAKE) -C overlay

clean:
	@$(MAKE) -C sysmodule clean
	@$(MAKE) -C overlay clean
	@rm -rf dist
