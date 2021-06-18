VERSION=7.0.6
REVISION=$(shell git rev-parse HEAD)

OUT = out
GEN = $(OUT)/gen
RES = $(OUT)/resources
WEB = $(OUT)/web
POSIX = $(OUT)/posix
WINDOWS = $(OUT)/windows
ESP32 = $(OUT)/esp32
DEPLOY = $(OUT)/deploy

ifneq ($(OS),Windows_NT)
  OS = $(shell uname -s)
endif

CFLAGS_COMMON = -O2 -I ./ -I $(OUT)

CFLAGS_MINIMIZE = \
                -DUEFORTH_MINIMAL \
                -fno-exceptions \
                -ffreestanding \
                -fno-stack-check \
                -fno-stack-protector \
                -fno-stack-protector \
                -fomit-frame-pointer \
                -mno-stack-arg-probe \
                -fno-ident \
                -ffunction-sections -fdata-sections \
                -fmerge-all-constants
CFLAGS = $(CFLAGS_COMMON) \
         $(CFLAGS_MINIMIZE) \
         -Wall \
         -Werror

ifeq ($(OS),Darwin)
  CFLAGS += -Wl,-dead_strip
else
  CFLAGS_MINIMIZE += -Wl,--build-id=none
  CFLAGS += -s -Wl,--gc-sections -no-pie -Wl,--build-id=none
endif

STRIP_ARGS = -S

ifeq ($(OS),Darwin)
 STRIP_ARGS += -x
else
 STRIP_ARGS += --strip-unneeded \
              --remove-section=.note.gnu.gold-version \
              --remove-section=.comment \
              --remove-section=.note \
              --remove-section=.note.gnu.build-id \
              --remove-section=.note.ABI-tag
endif

LIBS=-ldl

WIN_CFLAGS = $(CFLAGS_COMMON) \
             -I "c:/Program Files (x86)/Microsoft SDKs/Windows/v7.1A/Include" \
             -I "c:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Tools/MSVC/14.28.29333/include" \
             -I "c:/Program Files (x86)/Windows Kits/10/Include/10.0.19041.0/ucrt"

WIN_LFLAGS32 = /LIBPATH:"c:/Program Files (x86)/Microsoft SDKs/Windows/v7.1A/Lib" \
               /LIBPATH:"c:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Tools/MSVC/14.28.29333/lib/x86" \
               /LIBPATH:"c:/Program Files (x86)/Windows Kits/10/Lib/10.0.19041.0/ucrt/x86"

WIN_LFLAGS64 = /LIBPATH:"c:/Program Files (x86)/Microsoft SDKs/Windows/v7.1A/Lib/x64" \
               /LIBPATH:"c:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Tools/MSVC/14.28.29333/lib/x64" \
               /LIBPATH:"c:/Program Files (x86)/Windows Kits/10/Lib/10.0.19041.0/ucrt/x64" \

TARGETS = $(WEB)/terminal.html \
          $(WEB)/ueforth.js \
          $(POSIX)/ueforth \
          $(ESP32)/ESP32forth/ESP32forth.ino

LSQ = ls 2>/dev/null

PROGFILES = /mnt/c/Program Files (x86)
MSVS = "${PROGFILES}/Microsoft Visual Studio"
MSKITS = "${PROGFILES}/Windows Kits"
CL32 = "$(shell $(LSQ) ${MSVS}/*/*/VC/Tools/MSVC/*/bin/Hostx86/x86/cl.exe | head -n 1)"
CL64 = "$(shell $(LSQ) ${MSVS}/*/*/VC/Tools/MSVC/*/bin/Hostx86/x64/cl.exe | head -n 1)"
LINK32 = "$(shell $(LSQ) ${MSVS}/*/*/VC/Tools/MSVC/*/bin/Hostx86/x86/link.exe | head -n 1)"
LINK64 = "$(shell $(LSQ) ${MSVS}/*/*/VC/Tools/MSVC/*/bin/Hostx86/x64/link.exe | head -n 1)"
RC32 = "$(shell $(LSQ) ${MSKITS}/*/bin/*/x86/rc.exe | head -n 1)"
RC64 = "$(shell $(LSQ) ${MSKITS}/*/bin/*/x64/rc.exe | head -n 1)"

# Selectively enable windows if tools available
DEPLOYABLE := 1
ifneq ("", $(CL32))
  ifneq ("", $(RC32))
    TARGETS += $(WINDOWS)/uEf32.exe
  else
    $(warning "Missing Visual Studio rc.exe skipping 32-bit Windows.")
    DEPLOYABLE := 0
  endif
else
  $(warning "Missing Visual Studio cl.exe skipping 32-bit Windows.")
  DEPLOYABLE := 0
endif
ifneq ("", $(CL64))
  ifneq ("", $(RC64))
    TARGETS += $(WINDOWS)/uEf64.exe
  else
    $(warning "Missing Visual Studio rc.exe skipping 64-bit Windows.")
    DEPLOYABLE := 0
  endif
else
  $(warning "Missing Visual Studio cl.exe skipping 64-bit Windows.")
  DEPLOYABLE := 0
endif

# Decide if we can deploy.
DEPLOY_TARGETS =
ifeq (1, $(DEPLOYABLE))
  DEPLOY_TARGETS := $(DEPLOY)/app.yaml
else
  $(warning "Missing some platforms skipping deployment build.")
endif

all: $(TARGETS) tests $(DEPLOY_TARGETS)

clean:
	rm -rf $(OUT)

# ---- TESTS ----

tests: unit_tests see_all_test

unit_tests: $(POSIX)/ueforth common/all_tests.fs
	$^

see_all_test: $(POSIX)/ueforth
	echo "internals see-all bye" | $< >/dev/null

# ---- GENERATED ----

$(GEN):
	mkdir -p $@

POSIX_BOOT =  common/boot.fs common/conditionals.fs common/vocabulary.fs \
              common/hide_calls.fs common/ansi.fs \
              posix/posix.fs posix/posix_highlevel.fs posix/termios.fs common/locals.fs \
              common/tasks.fs common/utils.fs common/highlevel.fs common/filetools.fs \
              posix/posix_desktop.fs \
              common/streams.fs common/blocks.fs \
              posix/sockets.fs posix/telnetd.fs posix/httpd.fs posix/web_interface.fs \
              posix/autoboot.fs \
              common/fini.fs
$(GEN)/posix_boot.h: common/source_to_string.js $(POSIX_BOOT) | $(GEN)
	echo "ok" | cat $(POSIX_BOOT) - | $< boot $(VERSION) $(REVISION) >$@

WINDOWS_BOOT = common/boot.fs common/conditionals.fs common/vocabulary.fs \
               common/hide_calls.fs common/ansi.fs \
               windows/windows.fs windows/windows_highlevel.fs common/highlevel.fs \
               common/tasks.fs common/utils.fs common/filetools.fs common/streams.fs \
               common/blocks.fs common/locals.fs \
               common/fini.fs
$(GEN)/windows_boot.h: common/source_to_string.js $(WINDOWS_BOOT) | $(GEN)
	echo "ok" | cat $(WINDOWS_BOOT) - | $< boot $(VERSION) $(REVISION) >$@

ESP32_BOOT = common/boot.fs common/conditionals.fs common/vocabulary.fs \
             common/tasks.fs esp32/platform.fs esp32/highlevel.fs \
             esp32/bindings.fs common/highlevel.fs \
             common/filetools.fs common/utils.fs common/locals.fs \
             common/streams.fs posix/httpd.fs posix/web_interface.fs esp32/web_interface.fs \
             esp32/registers.fs esp32/timers.fs \
             esp32/bterm.fs posix/telnetd.fs \
             esp32/camera.fs esp32/camera_server.fs common/blocks.fs \
             esp32/autoboot.fs common/fini.fs
$(GEN)/esp32_boot.h: common/source_to_string.js $(ESP32_BOOT) | $(GEN)
	echo "ok" | cat $(ESP32_BOOT) - | $< boot $(VERSION) $(REVISION) >$@

$(GEN)/dump_web_opcodes: web/dump_web_opcodes.c common/opcodes.h | $(GEN)
	$(CC) $(CFLAGS) $< -o $@

$(GEN)/web_cases.js: $(GEN)/dump_web_opcodes | $(GEN)
	$< cases >$@

$(GEN)/web_dict.js: $(GEN)/dump_web_opcodes | $(GEN)
	$< dict >$@

# ---- RESOURCES ----

$(RES):
	mkdir -p $@

$(RES)/eforth16x16.png: images/eforth.png | $(RES)
	convert -resize 16x16 $< $@

$(RES)/eforth32x32.png: images/eforth.png | $(RES)
	convert -resize 32x32 $< $@

$(RES)/eforth48x48.png: images/eforth.png | $(RES)
	convert -resize 48x48 $< $@

$(RES)/eforth256x256.png: images/eforth.png | $(RES)
	convert -resize 256x256 $< $@

ICON_SIZES = $(RES)/eforth256x256.png \
             $(RES)/eforth48x48.png \
             $(RES)/eforth32x32.png \
             $(RES)/eforth16x16.png

$(RES)/eforth.ico: $(ICON_SIZES)
	convert $^ $< $@

$(RES)/ueforth_res32.res: windows/ueforth.rc $(RES)/eforth.ico
	$(RC32) /fo $@ $<

$(RES)/ueforth_res64.res: windows/ueforth.rc $(RES)/eforth.ico
	$(RC64) /fo $@ $<

# ---- WEB ----

$(WEB):
	mkdir -p $(WEB)

$(WEB)/terminal.html: web/terminal.html | $(WEB)
	cp $< $@

$(WEB)/ueforth.js: \
        web/fuse_web.js \
        web/web.template.js \
        common/boot.fs \
        $(GEN)/web_dict.js \
        $(GEN)/web_cases.js | $(WEB)
	$^ >$@

# ---- POSIX ----

$(POSIX):
	mkdir -p $@

$(POSIX)/ueforth: \
    posix/posix_main.c \
    common/opcodes.h \
    common/interp.h \
    common/core.h \
    $(GEN)/posix_boot.h | $(POSIX)
	$(CC) $(CFLAGS) $< -o $@ $(LIBS)
	strip $(STRIP_ARGS) $@

# ---- WINDOWS ----

$(WINDOWS):
	mkdir -p $@

$(WINDOWS)/uEf32.obj: \
    windows/windows_main.c \
    common/opcodes.h \
    common/core.h \
    windows/windows_interp.h \
    $(GEN)/windows_boot.h | $(WINDOWS)
	$(CL32) /c /Fo$@ $(WIN_CFLAGS) $<

$(WINDOWS)/uEf32.exe: \
    $(WINDOWS)/uEf32.obj \
    $(RES)/ueforth_res32.res | $(WINDOWS)
	$(LINK32) /OUT:$@ $(WIN_LFLAGS32) $^

$(WINDOWS)/uEf64.obj: \
    windows/windows_main.c \
    common/opcodes.h \
    common/core.h \
    windows/windows_interp.h \
    $(GEN)/windows_boot.h | $(WINDOWS)
	$(CL64) /c /Fo$@ $(WIN_CFLAGS) $<

$(WINDOWS)/uEf64.exe: \
    $(WINDOWS)/uEf64.obj \
    $(RES)/ueforth_res64.res | $(WINDOWS)
	$(LINK64) /OUT:$@ $(WIN_LFLAGS64) $^

# ---- ESP32 ----

$(ESP32)/ESP32forth:
	mkdir -p $@

ESP32_PARTS = esp32/template.ino \
              common/opcodes.h \
              common/calling.h \
              common/core.h \
              common/interp.h \
              $(GEN)/esp32_boot.h

$(ESP32)/ESP32forth/ESP32forth.ino: \
    esp32/fuse_ino.js $(ESP32_PARTS) | $(ESP32)/ESP32forth
	$< $(VERSION) $(REVISION) $(ESP32_PARTS) >$@

# ---- PACKAGE ----

$(ESP32)/ESP32forth.zip: $(ESP32)/ESP32forth/ESP32forth.ino
	cd $(ESP32) && zip -r ESP32forth.zip ESP32forth

# ---- DEPLOY ----

$(DEPLOY):
	mkdir -p $@

REPLACE = common/replace.js \
          COMMON=@site/common.html \
          POSIX_COMMON=@site/posix_common.html \
          DESKTOP_COMMON=@site/desktop_common.html \
          MENU=@site/menu.html \
          VERSION=${VERSION} \
          REVISION=${REVISION}
UE_REPLACE = $(REPLACE) FORTH=uEForth
ESP_REPLACE = $(REPLACE) FORTH=ESP32forth

$(DEPLOY)/app.yaml: $(ESP32)/ESP32forth.zip \
                    $(wildcard site/*.html) \
                    site/app.yaml \
                    site/eforth.go \
                    $(TARGETS) | $(DEPLOY)
	mkdir -p $(DEPLOY)
	cp -r site/static $(DEPLOY)/static
	cp -r site/downloads $(DEPLOY)/downloads
	cp site/*.go $(DEPLOY)/
	cp site/*.yaml $(DEPLOY)/
	cp -r $(ESP32)/ESP32forth.zip $(DEPLOY)/downloads
	cp -r $(POSIX)/ueforth $(DEPLOY)/downloads/ueforth.linux
	cp -r $(WINDOWS)/uEf32.exe $(DEPLOY)/downloads/uEf32.exe
	cp -r $(WINDOWS)/uEf64.exe $(DEPLOY)/downloads/uEf64.exe
	cp -r $(RES)/eforth.ico $(DEPLOY)/downloads/favicon.ico
	cp site/.gcloudignore $(DEPLOY)
	cat site/ESP32forth.html | $(ESP_REPLACE) >$(DEPLOY)/ESP32forth.html
	cat site/index.html | $(UE_REPLACE) >$(DEPLOY)/index.html
	cat site/linux.html | $(UE_REPLACE) >$(DEPLOY)/linux.html
	cat site/windows.html | $(UE_REPLACE) >$(DEPLOY)/windows.html
	cat site/internals.html | $(UE_REPLACE) >$(DEPLOY)/internals.html
	cat site/classic.html | $(UE_REPLACE) >$(DEPLOY)/classic.html

deploy: clean all
	cd out/deploy && gcloud app deploy -q --project esp32forth *.yaml
	cd out/deploy && gcloud app deploy -q --project eforth *.yaml
