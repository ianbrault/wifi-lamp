/*
** build.rs
*/

extern crate cbindgen;

use std::env;

fn main() {
    let crate_dir = env::var("CARGO_MANIFEST_DIR").unwrap();
    cbindgen::Builder::new()
        .with_src(format!("{}/src/lib.rs", crate_dir))
        .with_language(cbindgen::Language::C)
        .generate()
        .expect("unable to generate bindings")
        .write_to_file("src/lamp_ios_lib.h");
}
