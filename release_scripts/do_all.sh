#!/usr/bin/env sh

set -e

DIR="$( cd "$( dirname "$0" )" && pwd )"

# check for "--all" flag
if [ "$1" = "--all" ]; then
    echo "Building all prerequisites"
    sh "$DIR"/build_sphinx_docs.sh
    sh "$DIR"/package_python.sh
    sh "$DIR"/package_conda.sh
else
    if [ ! -d docs/build ]; then
        echo "Docs are not built."
        exit 1
    fi
    if [ ! -d dist ]; then
        echo "Python dist is not built."
        exit 1
    fi
    if [ ! -d output ]; then
        echo "Conda dist is not built."
        exit 1
    fi
fi

# make a dir called dist_site
rm -rf dist_site
mkdir dist_site

# copy python dist to dist_site
mkdir dist_site/dist-python
cp -r dist/* dist_site/dist-python/

# copy conda channel dist to dist_site
mkdir dist_site/dist-conda
cp -r output/* dist_site/dist-conda/

# copy the docs to the dist_site
mkdir dist_site/docs
cp -r docs/build/ dist_site/docs/