# Contributing to Oceanix

Thank you for your interest in contributing to Oceanix! We follow the Google C++ Style Guide to maintain code quality and consistency. Please review these guidelines before contributing.

## How to Contribute

1. **Fork** the repository and create a new branch for your feature or bugfix.
2. **Follow the code style** described in the [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html) and our project conventions.
3. **Write clear commit messages** and document your code.
4. **Open a pull request** and describe your changes.
5. For major changes, **open an issue** first to discuss your proposal.

## Branching and Pull Requests

- Use feature branches (e.g., `feature/controller-xyz` or `bugfix/sensor-init`).
- All code must be reviewed before merging to `main`.
- Ensure your branch is up to date with `main` before submitting a PR.

## Code Style

- Follow the [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html).
- Use `clang-format` if possible.
- Document all classes and functions with Doxygen-style comments.

## Debug Standard

Every class for debug must use the `print_log()` function:

```
[CLASS_N][TYPE ] msg...
// CLASS_N: 7 chars, TYPE: 5 chars (DEBUG | INFO_ | WARN_ | ERROR)
```

The `verbose` argument enables DEBUG messages. INFO, WARN, and ERROR are enabled by default.

## Adding New Modules

- **Controllers:** Implement the abstract controller interface in `include/controller_abstract.hpp` and add your implementation in `src/` and `include/`.
- **Sensors:** Extend the sensor interface in `include/sensor.hpp` and add your implementation.
- **Configuration:** Add new parameters to the relevant JSON files in `config/` and document them in [configuration.md](configuration.md).

## Tests and Documentation

- All new features must include tests (see `build_and_test.sh`).
- Update documentation in `README.md` and `docs/` as needed.

## Issue Reporting

- Use GitHub Issues for bugs, feature requests, and questions.
- Provide as much detail as possible (logs, OS, steps to reproduce).