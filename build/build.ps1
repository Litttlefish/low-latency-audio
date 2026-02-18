param(
    [string]$RepoRoot = "",
    [ValidateSet("Debug", "Release")]
    [string]$Configuration = "Release"
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

function Assert-MSBuildSuccess {
    param([string]$StepName)
    if ($LASTEXITCODE -ne 0) {
        Write-Host "ERROR: MSBuild failed for '$StepName'. Exit code $LASTEXITCODE" -ForegroundColor Red
        exit $LASTEXITCODE
    }
}

if ([string]::IsNullOrWhiteSpace($RepoRoot)) {
    $RepoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
}

if (-not (Test-Path (Join-Path $RepoRoot "src"))) {
    Write-Host "ERROR: '$RepoRoot' does not look like the low-latency-audio repo (missing 'src' folder)." -ForegroundColor Red
    exit 1
}

if (-not (Get-Command msbuild.exe -ErrorAction SilentlyContinue)) {
    Write-Host "ERROR: msbuild.exe not found in PATH. Please run from a VS Developer Command Prompt." -ForegroundColor Red
    exit 1
}

Write-Host "Repo root     : $RepoRoot"
Write-Host "Configuration : $Configuration"
Write-Host ""

if ($env:GITHUB_ACTIONS -eq 'true') {
    Write-Host "CI Environment detected. Ensuring WDK is installed..." -ForegroundColor Cyan
    $wdkExtensionPath = "$env:WindowsSdkDir\Vsix"
    if (-not (Test-Path $wdkExtensionPath)) {
        winget install --source winget --exact --id Microsoft.WindowsSDK.10.0.26100
        winget install --source winget --exact --id Microsoft.WindowsWDK.10.0.26100
        $vsix = Get-ChildItem -Path $wdkExtensionPath -Filter "WDK.vsix" -Recurse | Select-Object -First 1
        
        # if ($vsix -and (Test-Path $vsix.FullName)) {
        Write-Host "Installing WDK VSIX Extension..." -ForegroundColor Cyan
        Start-Process -FilePath "C:\Program Files (x86)\Microsoft Visual Studio\Installer\VSIXInstaller.exe" -ArgumentList "/q", "/admin", $vsix.FullName -Wait
        # }
    }
}

$sourceRoot = Join-Path $RepoRoot "src"
$vsfilesFolder = Join-Path $sourceRoot "vsfiles"
$vsfilesFolderOut = Join-Path $vsfilesFolder "out"
$stagingFolder = Join-Path $RepoRoot "build" "staging"
$releaseFolder = Join-Path $RepoRoot "build" "release"
$installerProjectFolder = Join-Path $sourceRoot "installer"

$acxSolution = Join-Path $sourceRoot "uac2-driver"        "USBAudioAcxDriver.sln"
$asioSolution = Join-Path $sourceRoot "uac2-asio"          "USBAsio.sln"
$controlPanelSolution = Join-Path $sourceRoot "asio-control-panel" "USBAsioControlPanel.sln"
$installerProject = Join-Path $installerProjectFolder          "asio-installer.sln"

foreach ($sln in @($acxSolution, $asioSolution, $controlPanelSolution, $installerProject)) {
    if (-not (Test-Path $sln)) {
        Write-Host "ERROR: Solution file not found: $sln" -ForegroundColor Red
        exit 1
    }
}

Write-Host "Creating / cleaning output folders..."

foreach ($dir in @($stagingFolder, $vsfilesFolder, $releaseFolder)) {
    if (Test-Path $dir) {
        Remove-Item "$dir\*" -Recurse -Force -ErrorAction SilentlyContinue
    }
    New-Item -Path $dir -ItemType Directory -Force | Out-Null
}

Write-Host ""

Write-Host "=== Restoring NuGet packages for ACX Driver ===" -ForegroundColor Cyan
msbuild.exe -t:restore $acxSolution -p:RestorePackagesConfig=true
Assert-MSBuildSuccess "ACX Driver NuGet restore"

foreach ($platform in @("x64", "ARM64")) {
    Write-Host ""
    Write-Host "--- Building ACX Driver: $Configuration | $platform ---" -ForegroundColor Yellow

    msbuild.exe -p:Platform=$platform `
        -p:Configuration=$Configuration `
        -verbosity:normal `
        -target:Rebuild `
        $acxSolution

    Assert-MSBuildSuccess "ACX Driver $Configuration|$platform"

    $driverOutputFolder = Join-Path $vsfilesFolderOut "USBAudioAcxDriver" $platform $Configuration "USBAudioAcxDriver"
    Write-Host "  Output: $driverOutputFolder"

    $stagingTarget = Join-Path $stagingFolder $platform $Configuration
    New-Item -Path $stagingTarget -ItemType Directory -Force | Out-Null

    Copy-Item -Path (Join-Path $driverOutputFolder "*.*") -Destination $stagingTarget -ErrorAction SilentlyContinue
}

Write-Host ""

Write-Host "=== Restoring NuGet packages for ASIO Driver ===" -ForegroundColor Cyan
msbuild.exe -t:restore $asioSolution -p:RestorePackagesConfig=true
Assert-MSBuildSuccess "ASIO Driver NuGet restore"

foreach ($platform in @("x64", "ARM64EC")) {
    Write-Host ""
    Write-Host "--- Building ASIO Driver: $Configuration | $platform ---" -ForegroundColor Yellow

    msbuild.exe -p:Platform=$platform `
        -p:Configuration=$Configuration `
        -verbosity:normal `
        -target:Rebuild `
        $asioSolution

    Assert-MSBuildSuccess "ASIO Driver $Configuration|$platform"

    $asioOutputFolder = Join-Path $vsfilesFolderOut "USBAsio" $platform $Configuration
    Write-Host "  Output: $asioOutputFolder"

    if ($platform -eq "ARM64EC") {
        $destinationPlatform = "ARM64"
    }
    else {
        $destinationPlatform = "x64"
    }

    $stagingTarget = Join-Path $stagingFolder $destinationPlatform $Configuration
    New-Item -Path $stagingTarget -ItemType Directory -Force | Out-Null

    Copy-Item -Path (Join-Path $asioOutputFolder "*.dll") -Destination $stagingTarget -ErrorAction SilentlyContinue
    Copy-Item -Path (Join-Path $asioOutputFolder "*.pdb") -Destination $stagingTarget -ErrorAction SilentlyContinue
}

Write-Host ""

Write-Host "=== Restoring NuGet packages for Control Panel ===" -ForegroundColor Cyan
msbuild.exe -t:restore $controlPanelSolution -p:RestorePackagesConfig=true
Assert-MSBuildSuccess "Control Panel NuGet restore"

foreach ($platform in @("x64", "ARM64")) {
    Write-Host ""
    Write-Host "--- Building Control Panel: $Configuration | $platform ---" -ForegroundColor Yellow

    msbuild.exe -p:Platform=$platform `
        -p:Configuration=$Configuration `
        -verbosity:normal `
        -target:Rebuild `
        $controlPanelSolution

    Assert-MSBuildSuccess "Control Panel $Configuration|$platform"

    $controlPanelOutputFolder = Join-Path $vsfilesFolderOut "USBAsioControlPanel" $platform $Configuration
    Write-Host "  Output: $controlPanelOutputFolder"

    $stagingTarget = Join-Path $stagingFolder $platform $Configuration
    New-Item -Path $stagingTarget -ItemType Directory -Force | Out-Null

    Copy-Item -Path (Join-Path $controlPanelOutputFolder "*.exe") -Destination $stagingTarget -ErrorAction SilentlyContinue
}

Write-Host ""

Write-Host "=== Building Installers ===" -ForegroundColor Cyan

foreach ($platform in @("x64", "ARM64")) {
    Write-Host ""
    Write-Host "--- Building Installer: $Configuration | $platform ---" -ForegroundColor Yellow

    $defineConstants = "StagingSourceRootFolder=$stagingFolder"

    msbuild.exe -p:Platform=$platform `
        -p:Configuration=$Configuration `
        -p:DefineConstants="$defineConstants" `
        -verbosity:normal `
        -target:Rebuild `
        $installerProject

    Assert-MSBuildSuccess "Installer $Configuration|$platform"

    $releaseTarget = Join-Path $releaseFolder $platform $Configuration
    New-Item -Path $releaseTarget -ItemType Directory -Force | Out-Null

    $installerBinFolder = Join-Path $installerProjectFolder "bin" $platform $Configuration
    Copy-Item -Path (Join-Path $installerBinFolder "*.msi") -Destination $releaseTarget -ErrorAction SilentlyContinue
}

Write-Host ""
Write-Host "=== Build complete ===" -ForegroundColor Green
Write-Host "Staging folder : $stagingFolder"
Write-Host "Release folder : $releaseFolder"

Write-Host ""






