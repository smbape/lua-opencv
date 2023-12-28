#!/bin/bash

PYTHON_VENV_PATH="$1"
PYTHONPATH="$2"
alias="$3"
zoo="$4"
models="$5"
script="$6"

if [ ! -d "$PYTHON_VENV_PATH" ]; then
    python -m venv "$PYTHON_VENV_PATH"
    source "$PYTHON_VENV_PATH/bin/activate" || exit $?

    python -m pip install --upgrade pip
    python -m pip install --upgrade opencv-python PyYAML requests
else
    source "$PYTHON_VENV_PATH/bin/activate" || exit $?
fi

[ -d "$models" ] || mkdir "$models" || exit $?
cd "$models" && PYTHONPATH="$PYTHONPATH" python "$script" $alias --zoo "$zoo"
