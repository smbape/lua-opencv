#requires -version 5.0

[CmdletBinding()]
param (
    [Parameter(Position=0, Mandatory)][string] $Model,
    [Parameter(Position=1, Mandatory)][string] $Venv,
    [Parameter(Position=2)][string] $Destination = "",
    [Parameter(Position=3)][string] $Zoo =  "",
    [Parameter(Position=4)][string] $Python = "python",
    [Parameter(Position=5)][string] $PythonPath = ""
)
# "powershell.exe -ExecutionPolicy UnRestricted -File $PSCommandPath"
# "pwsh.exe -ExecutionPolicy UnRestricted -File $PSCommandPath"

$ErrorActionPreference = "Stop"
Set-StrictMode -Version 3.0
trap { throw $Error[0] }

if ([string]::IsNullOrEmpty($Destination)) {
    $Destination = Join-Path $PSScriptRoot "models"
}

if ([string]::IsNullOrEmpty($Zoo)) {
    $Zoo = Join-Path $PSScriptRoot "models.yml"
}

$PYTHON_VENV_PATH = $Venv

if (!(Test-Path -Path $PYTHON_VENV_PATH)) {
    foreach($exe in (where.exe "$Python")) {
        $PythonCmd = Get-Command "$exe"
        break
    }

    Write-Host "$($PythonCmd.Source) -m venv $PYTHON_VENV_PATH"
    & $PythonCmd.Source -m venv "$PYTHON_VENV_PATH"
    attrib +h "$PYTHON_VENV_PATH"

    # Activate venv
    & "$PYTHON_VENV_PATH\Scripts\Activate.ps1"

    python -m pip install --upgrade pip
    python -m pip install --upgrade opencv-python PyYAML requests
} else {
    # Activate venv
    & "$PYTHON_VENV_PATH\Scripts\Activate.ps1"
}

if (!(Test-Path -Path $Destination)) {
    mkdir $Destination
}
cd "$Destination"

$Env:PYTHONPATH = $PythonPath
$script = Join-Path $PSScriptRoot download_model.py
python $script $Model --zoo $Zoo
