use std::ffi::CStr;
use std::fs::copy;
use std::fs::write;

static mut ASSETS: Vec<String> = Vec::new();

#[no_mangle]
pub extern "C" fn assetbundler_bundle(input_path: *const i8, output_path: *const i8) -> i32 {
    unsafe {
        let input = match CStr::from_ptr(input_path).to_str() {
            Ok(s) => s,
            Err(_) => return -1,
        };
        let output = match CStr::from_ptr(output_path).to_str() {
            Ok(s) => s,
            Err(_) => return -1,
        };

        if std::fs::create_dir_all(output).is_err() { return -1; }

        let mut manifest = String::new();

        for asset in &ASSETS {
            let src = format!("{}/{}", input, asset);
            let dst = format!("{}/{}", output, asset);
            if std::fs::copy(&src, &dst).is_err() { return -1; }
            manifest.push_str(&format!("{}\n", asset));
        }

        let manifest_path = format!("{}/manifest.txt", output);
        if std::fs::write(manifest_path, manifest).is_err() { return -1; }
    }
    0
}

#[no_mangle]
pub extern "C" fn assetbundler_add_asset(path: *const i8) -> i32 {
    unsafe {
        let s = match CStr::from_ptr(path).to_str() {
            Ok(s) => s,
            Err(_) => return -1,
        };
        ASSETS.push(s.to_string());
    }
    0
}

#[no_mangle]
pub extern "C" fn assetbundler_clear() {
    unsafe {
        ASSETS.clear();
    }
}
