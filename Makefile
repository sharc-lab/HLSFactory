clean:
	rm -rf **/__pycache__/
	rm -rf .mypy_cache/
	rm -rf .pytest_cache/
	rm -rf .ruff_cache/
	rm -rf dist/
	rm -rf .vscode/
	rm -rf .coverage
	rm -rf htmlcov/
	rm -rf output/
	rm -rf *.egg-info/
	rm -rf output_site/

PHONY: clean