const fs = require('fs');
const path = require('path');

console.log('Testing shared database setup...\n');

// Test Windows directory (for development)
const windowsDbPath = 'C:\\materia\\materia-v5.db';
const windowsBackupPath = 'C:\\materia\\backups';

// Test container directory (simulated)
const containerDbPath = '/materia/materia-v5.db';
const containerBackupPath = '/materia/backups';

console.log('Checking Windows paths:');
console.log(`  Database: ${windowsDbPath}`);
console.log(`  Backups: ${windowsBackupPath}`);

// Check if Windows directories exist
const windowsDbExists = fs.existsSync(windowsDbPath);
const windowsBackupExists = fs.existsSync(windowsBackupPath);

console.log(`  Database exists: ${windowsDbExists ? '✅' : '❌'}`);
console.log(`  Backup dir exists: ${windowsBackupExists ? '✅' : '❌'}`);

// Check if we can write to the directories
try {
  const testFile = path.join(windowsBackupPath, 'test-write.txt');
  fs.writeFileSync(testFile, 'test');
  fs.unlinkSync(testFile);
  console.log(`  Write access: ✅`);
} catch (error) {
  console.log(`  Write access: ❌ (${error.message})`);
}

console.log('\nChecking container paths:');
console.log(`  Database: ${containerDbPath}`);
console.log(`  Backups: ${containerBackupPath}`);

// Check if container directories exist (if running in container)
const containerDbExists = fs.existsSync(containerDbPath);
const containerBackupExists = fs.existsSync(containerBackupPath);

console.log(`  Database exists: ${containerDbExists ? '✅' : '❌'}`);
console.log(`  Backup dir exists: ${containerBackupExists ? '✅' : '❌'}`);

console.log('\nEnvironment check:');
console.log(`  NODE_ENV: ${process.env.NODE_ENV || 'undefined'}`);
console.log(`  Current working directory: ${process.cwd()}`);

console.log('\nRecommendations:');
if (!windowsBackupExists) {
  console.log('  ❌ Run setup-shared-db.ps1 as Administrator to create directories');
}
if (!windowsDbExists) {
  console.log('  ℹ️  Database will be created on first run');
}
if (process.env.NODE_ENV === 'production') {
  console.log('  ✅ Production mode - will use shared database');
} else {
  console.log('  ℹ️  Development mode - will use local database');
}

console.log('\nTest complete!');
