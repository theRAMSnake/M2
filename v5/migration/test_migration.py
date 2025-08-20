#!/usr/bin/env python3
"""
Test script for the migration functionality.
Uses real source data to test the migration process.
"""

import sqlite3
import json
import os
import tempfile
import shutil
from migrate import DatabaseMigrator

def create_test_new_database(db_path: str):
    """Create a test new database with required structure"""
    conn = sqlite3.connect(db_path)
    cursor = conn.cursor()
    
    # Create documents table (new v5 format)
    cursor.execute("""
        CREATE TABLE documents (
            id INTEGER PRIMARY KEY,
            path TEXT UNIQUE NOT NULL,
            data TEXT NOT NULL,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
            updated_at DATETIME DEFAULT CURRENT_TIMESTAMP
        )
    """)
    
    # Create index for efficient path queries
    cursor.execute("""
        CREATE INDEX idx_documents_path ON documents(path)
    """)
    
    conn.commit()
    conn.close()
    print(f"✅ Created test new database")

def test_migration():
    """Test the migration process with real source data"""
    print("🧪 Testing migration functionality with real data...")
    print("=" * 50)
    
    # Use real source data
    real_source_path = r"C:\infra\materia.bu1755644404.375228"
    
    if not os.path.exists(real_source_path):
        print(f"❌ Real source data not found: {real_source_path}")
        print("Please ensure the backup file exists in C:\\infra\\")
        return False
    
    # Create temporary new database file
    with tempfile.NamedTemporaryFile(suffix='.db', delete=False) as new_db:
        new_db_path = new_db.name
    
    try:
        # Create test new database
        create_test_new_database(new_db_path)
        
        # Run migration with real source data
        migrator = DatabaseMigrator(real_source_path, new_db_path)
        success = migrator.run_migration()
        
        if success:
            print("\n✅ Migration test completed successfully!")
        else:
            print("\n❌ Migration test failed!")
            return False
            
    except Exception as e:
        print(f"\n❌ Test failed with error: {e}")
        return False
    finally:
        # Clean up temporary file
        try:
            os.unlink(new_db_path)
        except:
            pass
    
    return True

if __name__ == "__main__":
    success = test_migration()
    exit(0 if success else 1)
