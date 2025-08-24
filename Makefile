.PHONY: all clean sysmodule test-client test install

all: sysmodule test-client

sysmodule:
	@echo "Building XMusic sysmodule..."
	@./build.sh

test-client:
	@echo "Building XMusic test client..."
	@./build_test_client.sh

test: all
	@echo "=== XMusic Build Test ==="
	@ls -lh sysmodule/xmusic.nso test_client.nro 2>/dev/null && echo "✅ All builds successful" || echo "❌ Build files missing"

clean:
	@echo "Cleaning build files..."
	@rm -rf sysmodule/build sysmodule/*.elf sysmodule/*.nso sysmodule/*.map
	@rm -rf dist
	@rm -f test_client.elf test_client.nro test_client.o test_client.d
	@echo "✅ Clean complete"

install: sysmodule
	@echo "Copy the following to your Switch SD card:"
	@echo "  dist/* -> SD card root"
	@echo "  test_client.nro -> SD:/switch/"
