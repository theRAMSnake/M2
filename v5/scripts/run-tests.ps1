# Test runner script for Materia v5 (PowerShell)
Write-Host "🧪 Running DatabaseService tests..." -ForegroundColor Green

# Install dependencies if needed
if (-not (Test-Path "node_modules")) {
    Write-Host "📦 Installing dependencies..." -ForegroundColor Yellow
    npm install
}

# Run tests
Write-Host "🚀 Starting tests..." -ForegroundColor Cyan
npm test

# Check if tests passed
if ($LASTEXITCODE -eq 0) {
    Write-Host "✅ All tests passed!" -ForegroundColor Green
} else {
    Write-Host "❌ Some tests failed!" -ForegroundColor Red
    exit 1
}
