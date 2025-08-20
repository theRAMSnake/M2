const fs = require('fs');
const path = require('path');

console.log('Verifying production database setup...\n');

// Production paths
const productionDbPath = '/materia/materia-v5.db';
const productionBackupPath = '/materia/backups';
const materiaDir = '/materia';

console.log('Checking production paths:');
console.log(`  Materia directory: ${materiaDir}`);
console.log(`  Database: ${productionDbPath}`);
console.log(`  Backups: ${productionBackupPath}`);

// Check if directories exist
const materiaExists = fs.existsSync(materiaDir);
const backupExists = fs.existsSync(productionBackupPath);
const dbExists = fs.existsSync(productionDbPath);

console.log(`  Materia dir exists: ${materiaExists ? '✅' : '❌'}`);
console.log(`  Backup dir exists: ${backupExists ? '✅' : '❌'}`);
console.log(`  Database exists: ${dbExists ? '✅' : '❌'}`);

// Check if we can write to the directories
try {
  const testFile = path.join(productionBackupPath, 'test-write.txt');
  fs.writeFileSync(testFile, 'test');
  fs.unlinkSync(testFile);
  console.log(`  Write access: ✅`);
} catch (error) {
  console.log(`  Write access: ❌ (${error.message})`);
}

console.log('\nEnvironment check:');
console.log(`  NODE_ENV: ${process.env.NODE_ENV || 'undefined'}`);
console.log(`  Current working directory: ${process.cwd()}`);

console.log('\nDocker volume check:');
console.log('  Expected volume mounts:');
console.log('    - /materia:/materia (shared directory)');
console.log('    - Database: /materia/materia-v5.db');
console.log('    - Backups: /materia/backups/');

console.log('\nRecommendations:');
if (!materiaExists) {
  console.log('  ❌ /materia directory does not exist - check Docker volume mount');
}
if (!backupExists) {
  console.log('  ❌ /materia/backups directory does not exist - create it or check permissions');
}
if (!dbExists) {
  console.log('  ℹ️  Database will be created on first run');
}
if (process.env.NODE_ENV !== 'production') {
  console.log('  ⚠️  NODE_ENV is not set to production');
}


console.log('\nProduction setup verification complete!');
