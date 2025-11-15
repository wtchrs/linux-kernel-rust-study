# Linux kernel with rust modules

## Submodules

```bash
git submodule update --init --recursive --depth=1
```

## Linux kernel

```bash
./make-linux.sh rustavailable
./make-linux.sh menuconfig
# Set `General setup > Rust support`
# Save and exit
./make-linux.sh
```

This will generate kernel image in the `./linux-build/arch/x86/boot/bzImage` file.

## BusyBox

```bash
./make-busybox.sh menuconfig
# Set `Settings > Build static binary (no shared libs)`
# If you are in Arch Linux, Unset `Networking Utilities > tc`
# Save and exit
./make-busybox.sh install
```

If you failed with ncurses header not found, apply patch with the following command and retry build:

```bash
git apply busybox-ncurses.patch
```

This will generate the `./initramfs` directory, which will be used for making initramfs image.

## Make initramfs image

```bash
./gen-initramfs.sh
```

You can find the initramfs image in the `./initramfs.cpio.gz` file.

## Launch kernel image using qemu

```bash
./qemu.sh
```

## CREDITS

- BusyBox ncurses patch from https://stackoverflow.com/a/78545186
