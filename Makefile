.PHONY: all clean sysmodule test-client controller test install

all: sysmodule test-client controller

sysmodule:
	@echo "Building XMusic sysmodule..."
	@./build.sh

test-client:
	@echo "Building XMusic test client..."
	@./build_test_client.sh

controller:
	@echo "Building XMusic controller..."
	@cd overlay && ./build_controller.sh

test: all
	@echo "=== XMusic Build Test ==="
	@ls -lh sysmodule/xmusic.nso test_client.nro overlay/XMusicController.nro 2>/dev/null && echo "✅ All builds successful" || echo "❌ Build files missing"

clean:
	@echo "Cleaning build files..."
	@rm -rf sysmodule/build sysmodule/*.elf sysmodule/*.nso sysmodule/*.map
	@rm -rf dist
	@rm -f test_client.elf test_client.nro test_client.o test_client.d
	@cd overlay && rm -rf build *.elf *.nro *.nacp
	@echo "✅ Clean complete"

install: sysmodule controller
	@echo "Copy the following to your Switch SD card:"
	@echo "  dist/* -> SD card root (sysmodule)"
	@echo "  test_client.nro -> SD:/switch/ (IPC test)"
	@echo "  overlay/XMusicController.nro -> SD:/switch/ (controller app)"
