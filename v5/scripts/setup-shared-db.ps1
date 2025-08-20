# PowerShell script to set up shared database directory structure
# Run this script as Administrator to create the necessary directories

Write-Host "Setting up shared database directory structure..." -ForegroundColor Green

# Create the main materia directory
$materiaDir = "C:\materia"
if (!(Test-Path $materiaDir)) {
    New-Item -ItemType Directory -Path $materiaDir -Force
    Write-Host "Created directory: $materiaDir" -ForegroundColor Yellow
} else {
    Write-Host "Directory already exists: $materiaDir" -ForegroundColor Yellow
}

# Create backups subdirectory
$backupsDir = "C:\materia\backups"
if (!(Test-Path $backupsDir)) {
    New-Item -ItemType Directory -Path $backupsDir -Force
    Write-Host "Created directory: $backupsDir" -ForegroundColor Yellow
} else {
    Write-Host "Directory already exists: $backupsDir" -ForegroundColor Yellow
}



# Set appropriate permissions (optional - uncomment if needed)
# $acl = Get-Acl $materiaDir
# $accessRule = New-Object System.Security.AccessControl.FileSystemAccessRule("Everyone","FullControl","ContainerInherit,ObjectInherit","None","Allow")
# $acl.SetAccessRule($accessRule)
# Set-Acl $materiaDir $acl

Write-Host "Shared database directory setup complete!" -ForegroundColor Green
Write-Host "Database: C:\materia\materia-v5.db" -ForegroundColor Cyan
Write-Host "Backups: C:\materia\backups\" -ForegroundColor Cyan
