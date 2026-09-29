# Contributing to StormByte-Database

Issues and pull requests belong on **this** repository only. Fork and open a PR against the branch named by the maintainers.

Repository coding rules are in [CODING_STYLE.md](CODING_STYLE.md). Read it before changing code. In short: C++26, tabs, the Database nested-namespace/Doxygen convention, StormByte 2.0 boundary types, DLL-safe out-of-line heap operations and focused tests.

By submitting a contribution you assign copyright in that contribution to the copyright holder of this repository (David C. Manuelda). The dual license in [LICENSE](LICENSE) can then apply to it.

Send only code you wrote and are free to assign. Do not send code owned by an employer, a third party or another project unless you already have the right to assign that copyright here. Each contributor is responsible for that clearance.

New `.hxx` / `.h` / `.hpp` / `.cxx` / `.cpp` / `.cc` / `.c` files must start with the license header used in this repository, unchanged. Do not invent a shorter banner. CMake, Markdown and other non-C++ files do not take that header.

## Pull Requests

- Keep each change focused. Do not mix unrelated dependency upgrades, license work or drive-by renames.
- Preserve the inheritance-oriented backend contract. SQLite, PostgreSQL and MariaDB are base classes for application schemas.
- Add or extend tests when changing observable behavior. Run the tests for every enabled backend that can be exercised.
- Keep Database standalone. Do not add a dependency on System to support tests or examples.
- Do not change dependency pins or add files under `thirdparty/` unless the change explicitly requires it.
- Do not update badges, versions or release tags as part of an ordinary feature or fix.
- Conventional Commits in English are preferred.
