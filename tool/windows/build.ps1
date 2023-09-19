param([ValidateSet("Release", "Debug")][String] $BuildType = "Release",
      [switch] $RunTest = $false)

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

if (-not (Test-Path $buildDir))
{
    New-Item -Path $buildDir -ItemType Directory | Out-Null
}

cmake -S $projectDir -B $buildDir -DBoost_USE_STATIC_LIBS=ON
cmake --build $buildDir --config $BuildType

if ($RunTest)
{
    $env:LIBFTP_TEST_SERVER_PATH = Join-Path $projectDir "test\server\server.py"
    $testDir = Join-Path $buildDir "test"
    ctest --test-dir $testDir --build-config $BuildType --verbose
}