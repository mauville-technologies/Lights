# Ozzadar Typegen

A crack at doing type generation based on schema similar to protocol buffers.
I wanted a low-friction way of doing this in CMake without having to rely on
compiled libraries or anything like that.

This will obviously be a lot less powerful than protobufs, but for my purposes
it should do quite well.

## Limitations
* Currently only does C++ bindings. It generates code that complies and uses my `ozz_binary` library.
* All structs and enum definitions must be at the top level. Nesting them would be a PITA.
* Generated code cannot reference another generated file. All related types should be defined in the same file
 
## Notes

1. I'll need to do some think through about how to order the definitions in the .h files.
   2. This is connected in spirit to how to have separate definition files referencing eachother (you dont)