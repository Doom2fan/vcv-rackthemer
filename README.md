# vcv-rackthemer
This library adds runtime theming functionality with support for multiple themes being used at the same time.  
The library is based on [svg_theme by Paul Dempsey](https://github.com/Paul-Dempsey/svg_theme). It improves upon it by implementing a `ThemedSvg` struct instead of modifying the SVG data in memory, and adding extra features where possible.  
[vcv-svghelper by Dustin Lacewell](https://github.com/dustinlacewell/vcv-svghelper) is also included for convenience, and has been modified to work seamlessly with `ThemedSvg`.

# Compilation
Unlike svg_theme, this library is not a single header. It must be built with CMake, and relies on RackSDK.cmake to use the Rack SDK.

# Usage
See the [documentation](docs/Theming.md) for details on authoring themeable SVGs and themes.
The library makes use of namespace to avoid polluting the global namespace and for convenience.

# Credits
VCV for SVG rendering and support code.  
Paul Dempsey for [`svg_theme`](https://github.com/Paul-Dempsey/svg_theme). The theming documentation is also mostly `svg_theme`'s, with a few changes related to the library's added features and changes  
Dustin Lacewell for [`vcv-svghelper`](https://github.com/dustinlacewell/vcv-svghelper).  