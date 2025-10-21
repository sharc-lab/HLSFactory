# Contributing to HLSFactory

Thanks for your interest in improving HLSFactory! This guide explains how to get your development environment ready, the workflow we use for changes, and where to find the tools you need to keep docs and code healthy.

## Getting Set Up

- Ensure you are using Python 3.12 or newer.
- Run `uv sync` inside your clone to create an isolated virtual environment and install the package with all declared extras.
- If you cannot use `uv`, fall back to `pip install -e ".[test,docs]"` inside a virtual environment of your choice.

## Contribution Workflow

1. Fork the repository and create a feature branch off `main`.
2. Make focused commits with clear messages; avoid mixing unrelated fixes.
3. Add or update tests alongside code changes when behaviour changes.
4. Update documentation or examples if the user experience is affected.
5. Run the validation steps listed below before opening a pull request.
6. Open the pull request with a concise summary of the change, testing performed, and any follow-up work you are deferring.

Keeping pull requests small and well-scoped makes reviews faster for everyone.

## Coding Standards

- Match the existing formatting and prefer descriptive naming.
- Type hints are encouraged throughout new code.
- Imports should follow the standard library → third-party → local grouping.
- Use docstrings for public functions and classes when behaviour is not obvious from the signature alone.
- Avoid commented-out code; remove it or move it to tests/examples.

We use Ruff for linting; see the configuration in `pyproject.toml` for the enabled rules and exceptions.

## Testing and Static Checks

Run the following before you submit your changes:

```bash
uv run pytest
uv run ruff check .
```

If you are not using `uv`, activate your environment and execute the equivalent `pytest` / `ruff` commands directly. When your change modifies packaging or build logic, run any affected demos or scripts (for example `uv run python demos/demo_full_flow_xilinx/full_flow_xilinx.py`) and document the results in the pull request.

## Building the Documentation

Documentation lives under `docs/` and is built with Sphinx + MyST. To preview your changes:

```bash
uv run sphinx-build -M html docs/source docs/build
```

If you prefer the make wrapper, use `uv run make -C docs html`. The generated site is written to `docs/build/html`; open `index.html` in your browser to review the updates.

## Filing Issues and Asking Questions

- Use GitHub issues to report bugs or request features. Include reproduction steps, environment information, and expected vs actual behaviour.
- Draft pull requests are welcome if you need early feedback.
- For time-sensitive questions, mention the maintainers in an issue so we can triage promptly.

We appreciate every fix, feature, and doc improvement—thanks for helping us grow HLSFactory.
