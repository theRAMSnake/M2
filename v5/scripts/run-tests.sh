#!/bin/bash

# Test runner script for Materia v5
echo "🧪 Running DatabaseService tests..."

# Install dependencies if needed
if [ ! -d "node_modules" ]; then
    echo "📦 Installing dependencies..."
    npm install
fi

# Run tests
echo "🚀 Starting tests..."
npm test

# Check if tests passed
if [ $? -eq 0 ]; then
    echo "✅ All tests passed!"
else
    echo "❌ Some tests failed!"
    exit 1
fi
