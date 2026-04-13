#!/bin/sh
# a wrapper for CMAKE_CROSSCOMPILING_EMULATOR
# exit 77 means "skip test" (mapped in CMake via SKIP_RETURN_CODE)
#
# before using this script / tests, the Android SDK emulator must be running and connected via adb
# e.g. "adb devices" should show a device.


ADB="${ADB:-$ANDROID_SDK_ROOT/platform-tools/adb}"

if [ ! -x "$ADB" ]; then
  echo "adb wrapper: adb not found at $ADB (set ADB=...)." >&2
  exit 77
fi

state="$("$ADB" get-state 2>/dev/null || true)"
if [ "$state" != "device" ]; then
  echo "adb wrapper: no connected Android device/emulator, skipping test." >&2
  exit 77
fi

binary="$1"; shift
device_path="/data/local/tmp/$(basename "$binary")"

"$ADB" push "$binary" "$device_path" >/dev/null || exit 77
"$ADB" shell "chmod +x \"$device_path\" && \"$device_path\" $*"
