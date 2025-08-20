# Materia V5 - Test Deployment Script (Windows PowerShell)
# This script deploys the application for testing purposes on Windows

param(
    [switch]$Force
)

# Stop on first error
$ErrorActionPreference = "Stop"

Write-Host "🚀 Starting Materia V5 Test Deployment..." -ForegroundColor Cyan

# Configuration
$APP_NAME = "materia-v5-test"
$PORT = 3000
$DATA_DIR = ".\data"
$LOGS_DIR = ".\logs"
$BACKUPS_DIR = ".\backups"
$MATERIA_DIR = "C:\materia"

# Function to print colored output
function Write-Status {
    param([string]$Message)
    Write-Host "[INFO] $Message" -ForegroundColor Blue
}

function Write-Success {
    param([string]$Message)
    Write-Host "[SUCCESS] $Message" -ForegroundColor Green
}

function Write-Warning {
    param([string]$Message)
    Write-Host "[WARNING] $Message" -ForegroundColor Yellow
}

function Write-Error {
    param([string]$Message)
    Write-Host "[ERROR] $Message" -ForegroundColor Red
}

# Check prerequisites
function Test-Prerequisites {
    Write-Status "Checking prerequisites..."
    
    # Check if Docker is installed
    try {
        $null = docker --version
        Write-Success "Docker is installed"
    }
    catch {
        Write-Error "Docker is not installed or not in PATH"
        Write-Host "Please install Docker Desktop for Windows" -ForegroundColor Yellow
        exit 1
    }
    
    # Check if Docker Compose is available
    try {
        $null = docker-compose --version
        Write-Success "Docker Compose is available"
    }
    catch {
        Write-Error "Docker Compose is not available"
        Write-Host "Please ensure Docker Compose is installed with Docker Desktop" -ForegroundColor Yellow
        exit 1
    }
    
    # Check if Docker is running
    try {
        $null = docker info
        Write-Success "Docker is running"
    }
    catch {
        Write-Error "Docker is not running"
        Write-Host "Please start Docker Desktop" -ForegroundColor Yellow
        exit 1
    }
    
    Write-Success "Prerequisites check passed"
}

# Create necessary directories
function New-Directories {
    Write-Status "Creating necessary directories..."
    
    $directories = @($DATA_DIR, $LOGS_DIR, $BACKUPS_DIR)
    
    foreach ($dir in $directories) {
        if (-not (Test-Path $dir)) {
            New-Item -ItemType Directory -Path $dir -Force | Out-Null
            Write-Success "Created directory: $dir"
        }
        else {
            Write-Status "Directory already exists: $dir"
        }
    }
    
    # Create materia directory if it doesn't exist
    if (-not (Test-Path $MATERIA_DIR)) {
        Write-Warning "Materia directory $MATERIA_DIR does not exist"
        Write-Status "Creating materia directory..."
        try {
            New-Item -ItemType Directory -Path $MATERIA_DIR -Force | Out-Null
            Write-Success "Created directory: $MATERIA_DIR"
        }
        catch {
            Write-Error "Failed to create $MATERIA_DIR. You may need to run as Administrator."
            exit 1
        }
    }
    
    Write-Success "Directories created"
}

# Generate password hashes if they don't exist
function New-PasswordHashes {
    Write-Status "Checking password hashes..."
    
    $snakeHashPath = Join-Path $MATERIA_DIR "snake.hash"
    $sevaHashPath = Join-Path $MATERIA_DIR "seva.hash"
    
    if (-not (Test-Path $snakeHashPath) -or -not (Test-Path $sevaHashPath)) {
        Write-Warning "Password hashes not found, generating..."
        
        $generateScriptPath = "scripts\generate-hashes.js"
        if (Test-Path $generateScriptPath) {
            try {
                # Run from the v5 root directory to access node_modules
                node $generateScriptPath
                Write-Success "Password hashes generated"
            }
            catch {
                Write-Error "Failed to generate password hashes"
                Write-Host "Please ensure Node.js is installed and the script exists" -ForegroundColor Yellow
                exit 1
            }
        }
        else {
            Write-Error "generate-hashes.js script not found at $generateScriptPath"
            exit 1
        }
    }
    else {
        Write-Success "Password hashes found"
    }
}

# Build and start the application
function Start-Application {
    Write-Status "Building and starting application..."
    
    # Stop existing containers
    try {
        docker-compose down 2>$null
        Write-Status "Stopped existing containers"
    }
    catch {
        Write-Status "No existing containers to stop"
    }
    
    # Build and start
    try {
        docker-compose up -d --build
        Write-Success "Application deployed"
    }
    catch {
        Write-Error "Failed to deploy application"
        Write-Host "Check Docker logs for more details" -ForegroundColor Yellow
        exit 1
    }
}

# Copy password hashes to container
function Copy-PasswordHashes {
    Write-Status "Copying password hashes to container..."
    
    $snakeHashPath = Join-Path $MATERIA_DIR "snake.hash"
    $sevaHashPath = Join-Path $MATERIA_DIR "seva.hash"
    $containerName = "materia-v5"
    
    # Wait a moment for container to be fully started
    Start-Sleep -Seconds 3
    
    try {
        # Copy snake hash
        if (Test-Path $snakeHashPath) {
            docker cp $snakeHashPath "${containerName}:/materia/snake.hash"
            Write-Success "Copied snake.hash to container"
        } else {
            Write-Warning "snake.hash not found at $snakeHashPath"
        }
        
        # Copy seva hash
        if (Test-Path $sevaHashPath) {
            docker cp $sevaHashPath "${containerName}:/materia/seva.hash"
            Write-Success "Copied seva.hash to container"
        } else {
            Write-Warning "seva.hash not found at $sevaHashPath"
        }
        
        # Restart container to load the new hashes
        Write-Status "Restarting container to load password hashes..."
        docker-compose restart
        Start-Sleep -Seconds 2
        
        Write-Success "Password hashes copied and container restarted"
    }
    catch {
        Write-Error "Failed to copy password hashes to container"
        Write-Host "Error: $_" -ForegroundColor Yellow
    }
}

# Wait for application to be ready
function Wait-ForReady {
    Write-Status "Waiting for application to be ready..."
    
    $maxAttempts = 30
    $attempt = 1
    
    while ($attempt -le $maxAttempts) {
        try {
            $response = Invoke-WebRequest -Uri "http://localhost:$PORT/health" -TimeoutSec 5 -UseBasicParsing
            if ($response.StatusCode -eq 200) {
                Write-Success "Application is ready!"
                return $true
            }
        }
        catch {
            # Ignore errors and continue
        }
        
        Write-Status "Attempt $attempt/$maxAttempts - waiting..."
        Start-Sleep -Seconds 2
        $attempt++
    }
    
    Write-Error "Application failed to start within expected time"
    Write-Host "Check the logs with: docker-compose logs -f" -ForegroundColor Yellow
    return $false
}

# Display deployment information
function Show-DeploymentInfo {
    Write-Host ""
    Write-Host "🎉 Materia V5 Test Deployment Complete!" -ForegroundColor Green
    Write-Host ""
    Write-Host "📋 Deployment Information:" -ForegroundColor Cyan
    Write-Host "   Application: $APP_NAME"
    Write-Host "   Port: $PORT"
    Write-Host "   Data Directory: $DATA_DIR"
    Write-Host "   Logs Directory: $LOGS_DIR"
    Write-Host "   Password Hashes: Copied from $MATERIA_DIR"
    Write-Host ""
    Write-Host "🌐 Access URLs:" -ForegroundColor Cyan
    Write-Host "   Snake's Interface: http://localhost:$PORT/snake"
    Write-Host "   Seva's Interface: http://localhost:$PORT/seva"
    Write-Host "   API: http://localhost:$PORT/api"
    Write-Host "   Health Check: http://localhost:$PORT/health"
    Write-Host ""
    Write-Host "📝 Useful Commands:" -ForegroundColor Cyan
    Write-Host "   View logs: docker-compose logs -f"
    Write-Host "   Stop: docker-compose down"
    Write-Host "   Restart: docker-compose restart"
    Write-Host ""
}

# Main deployment process
function Main {
    Write-Host "==========================================" -ForegroundColor Magenta
    Write-Host "   Materia V5 Test Deployment Script" -ForegroundColor Magenta
    Write-Host "   (Windows PowerShell Version)" -ForegroundColor Magenta
    Write-Host "==========================================" -ForegroundColor Magenta
    Write-Host ""
    
    Test-Prerequisites
    New-Directories
    New-PasswordHashes
    Start-Application
    Copy-PasswordHashes
    
    if (Wait-ForReady) {
        Show-DeploymentInfo
    }
    else {
        Write-Host ""
        Write-Host "❌ Deployment completed with warnings" -ForegroundColor Yellow
        Write-Host "Check the logs and try accessing the application manually" -ForegroundColor Yellow
    }
}

# Run main function
Main
