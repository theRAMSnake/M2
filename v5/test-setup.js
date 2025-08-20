#!/usr/bin/env node

const fs = require('fs');
const path = require('path');

console.log('🧪 Testing Materia V5 Setup...\n');

// Check if required directories exist
const requiredDirs = ['src/backend', 'webapp', 'data', 'logs', 'backups'];
const missingDirs = [];

requiredDirs.forEach(dir => {
  if (!fs.existsSync(dir)) {
    missingDirs.push(dir);
  }
});

if (missingDirs.length > 0) {
  console.log('❌ Missing directories:');
  missingDirs.forEach(dir => console.log(`   - ${dir}`));
  console.log('\nPlease run: mkdir -p ' + missingDirs.join(' '));
} else {
  console.log('✅ All required directories exist');
}

// Check if package.json files exist
const packageFiles = ['package.json', 'webapp/package.json'];
const missingPackages = [];

packageFiles.forEach(file => {
  if (!fs.existsSync(file)) {
    missingPackages.push(file);
  }
});

if (missingPackages.length > 0) {
  console.log('❌ Missing package.json files:');
  missingPackages.forEach(file => console.log(`   - ${file}`));
} else {
  console.log('✅ All package.json files exist');
}

// Check if key files exist
const keyFiles = [
  'src/backend/server.ts',
  'webapp/app/page.tsx',
  'docker-compose.yml',
  'Dockerfile'
];

const missingFiles = [];

keyFiles.forEach(file => {
  if (!fs.existsSync(file)) {
    missingFiles.push(file);
  }
});

if (missingFiles.length > 0) {
  console.log('❌ Missing key files:');
  missingFiles.forEach(file => console.log(`   - ${file}`));
} else {
  console.log('✅ All key files exist');
}

// Check if materia directory exists
if (!fs.existsSync('/materia')) {
  console.log('⚠️  /materia directory not found');
  console.log('   Run: sudo mkdir -p /materia');
  console.log('   Then generate password hashes with: node scripts/generate-hashes.js');
} else {
  console.log('✅ /materia directory exists');
}

console.log('\n🎯 Next steps:');
console.log('1. Install dependencies: npm install && cd webapp && npm install');
console.log('2. Generate password hashes: node scripts/generate-hashes.js');
console.log('3. Start development: npm run dev');
console.log('4. Access at: http://localhost:3000/snake or http://localhost:3000/seva');
