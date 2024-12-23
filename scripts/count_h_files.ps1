$path = "E:\Projects\Lights"
$filter = "*.cpp" # Replace with your specific filetype
$excludeFolders = @(
    "cmake-build-debug",
    "cmake-build-release",
    "dist",
    ".git",
    ".idea",
    "shared\third_party"
)

$files = Get-ChildItem -Path $path -Filter $filter -Recurse | Where-Object {
    $fullPath = $_.Directory.FullName
    $isExcluded = $false
    
    foreach ($excludeFolder in $excludeFolders) {
        if ($fullPath -like "*\$excludeFolder*") {
            $isExcluded = $true
            break
        }
    }
    
    -not $isExcluded
}

$files.Count
