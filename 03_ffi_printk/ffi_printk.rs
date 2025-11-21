// SPDX-License-Identifier: GPL-2.0
//! Simple FFI example for the kernel rust module.
use kernel::{prelude::*, str::CString};

module! {
    type: RustKernelPrintkModule,
    name: "ffi_printk",
    authors: ["wtchrs"],
    description: "Rust kernel module example",
    license: "GPL",
}

extern "C" {
    //! Using `pr-...` macros is much simple and safe.
    fn _printk(fmt: *const c_char, ...) -> c_int;
}

//! Use `pr-...` macros
macro_rules! printk {
    ($($args:expr),*) => {_printk($($args),*)}
}

fn rust_printk(message: &str) {
    let cstr = CString::try_from_fmt(fmt!("{}", message)).unwrap();
    unsafe {
        printk!(cstr.as_ptr());
    }
}

struct RustKernelPrintkModule;

impl kernel::Module for RustKernelPrintkModule {
    fn init(_module: &'static ThisModule) -> kernel::error::Result<Self> {
        rust_printk("Hello, Rust kernel module!\n");
        Ok(RustKernelPrintkModule)
    }
}

impl Drop for RustKernelPrintkModule {
    fn drop(&mut self) {
        rust_printk("Goodbye, Rust kernel module!\n");
    }
}
