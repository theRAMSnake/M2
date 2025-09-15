/* Copies TinyMCE distribution from node_modules to public/tinymce for self-hosting */
const fs = require('fs');
const path = require('path');

const SRC_DIR = path.resolve(__dirname, '..', 'node_modules', 'tinymce');
const DEST_DIR = path.resolve(__dirname, '..', 'public', 'tinymce');

function copyRecursiveSync(src, dest) {
  if (!fs.existsSync(src)) return;
  const stat = fs.statSync(src);
  if (stat.isDirectory()) {
    if (!fs.existsSync(dest)) {
      fs.mkdirSync(dest, { recursive: true });
    }
    const entries = fs.readdirSync(src, { withFileTypes: true });
    for (const entry of entries) {
      const srcPath = path.join(src, entry.name);
      const destPath = path.join(dest, entry.name);
      if (entry.isDirectory()) {
        copyRecursiveSync(srcPath, destPath);
      } else if (entry.isFile()) {
        fs.copyFileSync(srcPath, destPath);
      }
    }
  } else if (stat.isFile()) {
    const destParent = path.dirname(dest);
    if (!fs.existsSync(destParent)) fs.mkdirSync(destParent, { recursive: true });
    fs.copyFileSync(src, dest);
  }
}

function main() {
  if (!fs.existsSync(SRC_DIR)) {
    console.error('TinyMCE not found at', SRC_DIR);
    process.exit(0);
  }
  // Only copy required items for our config
  const itemsToCopy = [
    'tinymce.min.js',
    'skins',
    'plugins',
    'themes',
    'icons',
    'models'
  ];

  if (!fs.existsSync(DEST_DIR)) fs.mkdirSync(DEST_DIR, { recursive: true });

  for (const item of itemsToCopy) {
    const src = path.join(SRC_DIR, item);
    const dest = path.join(DEST_DIR, item);
    copyRecursiveSync(src, dest);
  }

  console.log('TinyMCE assets copied to', DEST_DIR);
}

main();


