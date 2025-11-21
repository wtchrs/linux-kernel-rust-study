//! Rust device example
//! See more: https://rust-exercises.ferrous-systems.com/latest/book/building-linux-kernel-driver

use kernel::{
    device::Device,
    error,
    fs::{File, Kiocb},
    iov::IovIterDest,
    miscdevice::{MiscDeviceOptions, MiscDeviceRegistration},
    prelude::*,
    str::CString,
    types::ARef,
    uapi::get_random_bytes,
};

module! {
    type: RandomDeviceModule,
    name: "rust_random",
    authors: ["wtchrs"],
    description: "Rust random-like device module example",
    license: "GPL",
}

#[pin_data(PinnedDrop)]
struct RandomDeviceModule {
    #[pin]
    _miscdev: MiscDeviceRegistration<RandomDevice>,
}

// Implement `kernel::InPlaceModule` instead of `kernel::Module`.
impl kernel::InPlaceModule for RandomDeviceModule {
    fn init(_module: &'static ThisModule) -> impl pin_init::PinInit<Self, error::Error> {
        pr_info!("Rust random device module (init)\n");

        let options = MiscDeviceOptions {
            // Use "/dev/rust-random" as Misc device name
            name: kernel::c_str!("rust-random"),
        };

        try_pin_init!(Self {
            _miscdev <- MiscDeviceRegistration::register(options)
        })
    }
}

#[pinned_drop]
impl PinnedDrop for RandomDeviceModule {
    fn drop(self: Pin<&mut Self>) {
        pr_info!("Rust random device module (exit)\n");
    }
}

#[pin_data]
struct RandomDevice {
    dev: ARef<Device>,
}

static mut COMPLETE: bool = false;

#[vtable]
impl kernel::miscdevice::MiscDevice for RandomDevice {
    type Ptr = Pin<KBox<Self>>;

    fn open(file: &File, misc: &MiscDeviceRegistration<Self>) -> Result<Self::Ptr> {
        let dev = ARef::from(misc.device());

        dev_info!(
            dev,
            "Opening rust random device (uid = {})\n",
            file.cred().euid().into_uid_in_current_ns()
        );

        KBox::try_pin_init(try_pin_init!(RandomDevice { dev }), GFP_KERNEL)
    }

    fn release(device: Self::Ptr, file: &File) {
        dev_info!(
            *device.dev,
            "Closing rust random device (uid = {})\n",
            file.cred().euid().into_uid_in_current_ns()
        );
    }

    fn read_iter(kiocb: Kiocb<'_, Self::Ptr>, iov: &mut IovIterDest<'_>) -> Result<usize> {
        let me = kiocb.file();
        dev_info!(me.dev, "Read from rust random device\n");

        if unsafe { COMPLETE } {
            return Ok(0);
        }

        let mut rand_val: u64 = 0;
        unsafe {
            let rand_ptr: *mut u64 = &mut rand_val;
            get_random_bytes(rand_ptr as *mut c_void, size_of::<u64>());
        }

        if let Ok(str_to_send) = CString::try_from_fmt(fmt!("{}", rand_val)) {
            let bytes_to_send = str_to_send.to_bytes();
            let bytes_copied = iov.copy_to_iter(bytes_to_send);
            unsafe {
                COMPLETE = true;
            }
            Ok(bytes_copied)
        } else {
            dev_err!(me.dev, "Failed to convert random value as string\n");
            Err(EIO)
        }
    }
}
