# Prophet2012

Prophet2012 is an editor and librarian for the Sequential Circuits SCI samplers **Prophet 2000** and **Prophet 2002**. It transfers samples, sounds, maps and presets between the sampler and the computer via MIDI and lets you edit them on screen.

## Download

The current macOS version is available on the [releases page](https://github.com/wusel76/Prophet2012/releases):

- **v1.5 beta** – universal binary for Apple Silicon and Intel Macs, macOS 12 or later

Open `Prophet2012.dmg` and drag Prophet2012 to Applications. The app is not notarized, so macOS blocks the first start: right click on the app → Open, or allow it in System Settings → Privacy & Security.

## Documentation

The [Prophet2012 user manual](docs/manual/README.md) describes all functions with screenshots. The original Word document is in [docs](docs/Prophet2012_Manual.doc).

## Building

- **macOS:** Xcode project and build scripts are included, see [Install_MacOS.md](Install_MacOS.md). The third party libraries (wxWidgets, PortAudio, PortMidi, libsamplerate) are downloaded and built automatically.
- **Windows:** there is currently no build project. The Windows specific sources (Win32 MIDI and serial port) are still contained in `SRCDIR`.

## Background

The GUI was originally based on wxWidgets 2.8.12, the layout had been created with wxFormBuilder 3.3.4 (I believe) – there might be some hardcoded tweaks in the implementation. The macOS version now uses wxWidgets 3.2; Mac specific adaptions of the layout are in `SRCDIR/macstyle.mm`.

## Contributing

Contributions are welcome – please open an issue or a pull request.

## License

I am releasing this software under the GNU Affero General Public License, version 3 (AGPL-3.0); see [LICENSE](LICENSE). My intent is that further releases and changes remain open source in accordance with this license. I also ask that further releases of this software or final product remain free of charge, without paywalls or payments. This is my wish rather than an additional license restriction: AGPL-3.0 permits charging for the software.
