#!/usr/bin/env node

/**
 * Generate password hashes for Materia V5 users
 * This script creates bcrypt hashes for "snake" and "seva" users
 */

const fs = require('fs');
const path = require('path');

// Try to load bcrypt from parent directory's node_modules
let bcrypt;
try {
    bcrypt = require('bcrypt');
} catch (error) {
    // If not found, try parent directory
    const parentNodeModules = path.join(__dirname, '..', 'node_modules', 'bcrypt');
    if (fs.existsSync(parentNodeModules)) {
        bcrypt = require(parentNodeModules);
    } else {
        console.error('❌ bcrypt module not found!');
        console.log('💡 Please run: npm install');
        process.exit(1);
    }
}

// Configuration
const MATERIA_DIR = process.platform === 'win32' ? 'C:\\materia' : '/materia';
const SALT_ROUNDS = 12;

// Password storage
const PASSWORDS = {};

// Function to prompt for password
function promptForPassword(username) {
    const readline = require('readline');
    const rl = readline.createInterface({
        input: process.stdin,
        output: process.stdout
    });
    
    return new Promise((resolve) => {
        rl.question(`Enter password for ${username}: `, (password) => {
            rl.close();
            resolve(password);
        });
    });
}

function generateHash(password) {
    return bcrypt.hashSync(password, SALT_ROUNDS);
}

function saveHash(username, hash) {
    const hashPath = path.join(MATERIA_DIR, `${username}.hash`);
    
    try {
        // Create materia directory if it doesn't exist
        if (!fs.existsSync(MATERIA_DIR)) {
            fs.mkdirSync(MATERIA_DIR, { recursive: true });
            console.log(`Created directory: ${MATERIA_DIR}`);
        }
        
        // Write hash to file
        fs.writeFileSync(hashPath, hash);
        console.log(`✅ Generated hash for ${username}: ${hashPath}`);
        
        return true;
    } catch (error) {
        console.error(`❌ Failed to save hash for ${username}:`, error.message);
        return false;
    }
}

async function main() {
    console.log('🔐 Generating password hashes for Materia V5...');
    console.log(`📁 Materia directory: ${MATERIA_DIR}`);
    console.log('');
    
    let success = true;
    
    // Get passwords from user
    console.log('Please enter passwords for each user:');
    console.log('');
    
    PASSWORDS.snake = await promptForPassword('snake');
    PASSWORDS.seva = await promptForPassword('seva');
    
    console.log('');
    
    // Generate hashes for each user
    for (const [username, password] of Object.entries(PASSWORDS)) {
        console.log(`Generating hash for ${username}...`);
        
        const hash = generateHash(password);
        const saved = saveHash(username, hash);
        
        if (!saved) {
            success = false;
        }
        
        console.log(`   Hash: ${hash.substring(0, 20)}...`);
        console.log('');
    }
    
    if (success) {
        console.log('🎉 All password hashes generated successfully!');
        console.log('');
        console.log('📝 Next steps:');
        console.log('   1. Change the default passwords in production');
        console.log('   2. Update the passwords in this script if needed');
        console.log('   3. Ensure the hash files are secure');
        console.log('');
        console.log('🔒 Security note: Passwords are securely hashed and stored!');
    } else {
        console.log('❌ Some hashes failed to generate. Check the errors above.');
        process.exit(1);
    }
}

// Handle command line arguments for custom passwords
if (process.argv.length > 2) {
    const args = process.argv.slice(2);
    
    if (args.length === 2) {
        // Usage: node generate-hashes.js <snake_password> <seva_password>
        PASSWORDS.snake = args[0];
        PASSWORDS.seva = args[1];
        console.log('Using passwords from command line arguments');
    } else {
        console.log('Usage: node generate-hashes.js [snake_password] [seva_password]');
        console.log('If no passwords provided, you will be prompted to enter them');
    }
}

// Run the script
main();
