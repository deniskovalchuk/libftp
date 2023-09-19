function Get-Directory
{
    param([Parameter(Mandatory = $true)][String] $Path)

    Split-Path -Parent $Path
}

$scriptPath = $PSCommandPath
$toolWinDir = Get-Directory($scriptPath)
$toolDir = Get-Directory($toolWinDir)
$projectDir = Get-Directory($toolDir)
$buildDir = Join-Path $projectDir "build"

if (Test-Path $buildDir)
{
    Remove-Item $buildDir -Recurse -Force
}