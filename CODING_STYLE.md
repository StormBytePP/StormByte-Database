# StormByte-Database Coding Style

This is the coding style for StormByte-Database. Match nearby code when a detail is not covered here.

## Files and Formatting

- Headers use `.hxx` / `.h`; C++ implementations use `.cxx`.
- Every edited file ends with a newline.
- Indent with tabs. Do not use spaces for indentation or mix spaces into indentation.
- Do not run formatters that rewrite code outside the intended change. Preserve the repository's layout and align trailing Doxygen comments in columns.
- Include StormByte headers first, alphabetically, then a blank line and standard-library headers, alphabetically.
- Do not put `using namespace` in headers. In `.cxx` files, follow the local `using namespace StormByte::...;` pattern where appropriate.

## Namespaces and Doxygen

- In headers, use nested namespace blocks for namespace depth up to three. Do not use `namespace A::B::C { ... }` for these namespaces.
- Document every namespace block with Doxygen. Use the same qualified namespace name and identical brief for that namespace in every header.
- Each nested namespace body is indented one tab beyond its parent. Classes are one tab inside their namespace; access labels are one tab inside the class; members are one tab inside an access section.
- Public and private declarations, constructors, overloads, members and implementation classes need complete Doxygen. `= delete` declarations are the exception.
- Align member and alias `///<` comments to one column when the declarations fit on one line.
- Do not duplicate a Doxygen block when an overload or declaration is removed.

## C++ and Public API

- Use StormByte concepts, aliases, helpers and exceptions when Base provides them. Prefer `StormByte::Type::SameAs`, `StormByte::Expected` and Database exceptions over equivalent ad-hoc constructs.
- Use `StormByte::Size` for counts and indices; use `StormByte::ByteSize` for byte lengths. Convert to `std::size_t` only at the STL or C API boundary that requires it.
- Public text input uses `std::string_view`. Owned public text uses `StormByte::String::String`. Public byte payloads use `StormByte::BinaryData`.
- Do not expose `std::string`, `std::vector<std::byte>`, `CString` or `WCString` in the C++ public API. Standard strings and vectors may be used internally when their allocation and destruction stay in the owning module.
- Query and prepared-statement APIs take a single `std::string_view` instead of overloads for each string type.
- Keep all backend extension points and protected constructors/hooks usable by derived application classes.
- `Row` and `Rows` remain STL/ranges-friendly. Their iterators support standard algorithms; operations that allocate or release storage run out-of-line in the Database module.

## DLL and CRT Boundary

- Assume the consumer and Database may use different CRTs or standard libraries. Public signatures and object operations must not transfer allocator ownership across that boundary.
- Constructors, destructors, copy/move operations and methods that allocate, free or mutate module-owned heap state are defined out-of-line in Database sources.
- Ordinary `inline` is only an optimization hint. Use `STORMBYTE_FORCE_INLINE` only when the caller must execute work in its own CRT.
- Keep exported class visibility on the class declaration. Use the Database visibility macros for exported free functions.
- Do not put C-only wrappers in the public API. Generic template work that must remain in the caller is acceptable only when it does not violate heap ownership or the public type contract.

## BuildMaster and Dependencies

- The root `CMakeLists.txt` adds the root `buildmaster` directory before `lib` and owns the `BUILD_SHARED_LIBS` option.
- Declare the main library with `buildmaster_component(... BACKEND=host ...)`; its third argument is the list of source files.
- Put common source discovery in one glob. Add optional backend sources after target creation with `target_sources` under the matching backend condition.
- Put `buildmaster_link` where the linked component is defined. Do not add duplicate `target_link_libraries` calls between BuildMaster components.
- Vendor CMake owns vendor discovery/build. Static private vendor closure is flattened by BuildMaster; users do not repack archives.
- Install headers and targets from the component CMake. BuildMaster does not install component headers automatically.

## Tests and Documentation

- Keep tests honest about which backends they exercise and which services they require.
- Use the existing test files and test-registration patterns. Keep temporary build trees outside the user-reserved `build/` directory.
- For behavior or public API changes, update `CHANGELOG.md` and keep `README.md` claims and examples accurate.
- Keep the README suite table alphabetical and include every suite module, including String.
- Document the shared/static behavior and BuildMaster's static dependency flattening without presenting internal BuildMaster backend names as user-facing backends.
- The license is dual. `LICENSE` describes the dual terms; `COPYING.LGPLv3` contains the LGPL text. Third-party sources keep their own licenses.
