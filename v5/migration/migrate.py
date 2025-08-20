#!/usr/bin/env python3
"""
Migration script to transfer data from old Materia database to new v5 database structure.
Supports migrating BusinessEvent data from finance materia object children.
"""

import sqlite3
import json
import sys
import os
import argparse
from typing import List, Dict, Any
from datetime import datetime
import uuid

class DatabaseMigrator:
    def __init__(self, old_db_path: str, new_db_path: str):
        self.old_db_path = old_db_path
        self.new_db_path = new_db_path
        self.old_conn = None
        self.new_conn = None
        
    def connect_databases(self):
        """Connect to both old and new databases"""
        try:
            # Connect to old database
            self.old_conn = sqlite3.connect(self.old_db_path)
            self.old_conn.row_factory = sqlite3.Row
            
            # Connect to new database
            self.new_conn = sqlite3.connect(self.new_db_path)
            self.new_conn.row_factory = sqlite3.Row
            
            print(f"✅ Connected to old database: {self.old_db_path}")
            print(f"✅ Connected to new database: {self.new_db_path}")
            
        except Exception as e:
            print(f"❌ Error connecting to databases: {e}")
            sys.exit(1)
    
    def migrate_business_events(self) -> int:
        """Migrate BusinessEvent data from old database to new database"""
        migrated_count = 0
        
        try:
            cursor = self.old_conn.cursor()
            
            # Find the finance parent object in the objects table
            finance_id = 'finance'
            cursor.execute("SELECT Id, Json FROM objects WHERE Id = ?", (finance_id,))
            finance_result = cursor.fetchone()
            
            if not finance_result:
                print("⚠️  No finance object found in objects table")
                return 0
            
            print(f"✅ Found finance object with ID: {finance_id}")
            
            # Find all children through hierarchy connections
            cursor.execute("SELECT Id, Json FROM connections")
            all_connections = cursor.fetchall()
            
            hierarchy_children = []
            for conn_row in all_connections:
                try:
                    conn_data = json.loads(conn_row[1])
                    if (conn_data.get('A') == finance_id and 
                        conn_data.get('type') == 'Hierarchy'):
                        child_id = conn_data.get('B')
                        if child_id:
                            hierarchy_children.append(child_id)
                except json.JSONDecodeError:
                    continue
            
            print(f"📊 Found {len(hierarchy_children)} hierarchy children")
            
            # Migrate each child as a business event
            for child_id in hierarchy_children:
                cursor.execute("SELECT Id, Json FROM objects WHERE Id = ?", (child_id,))
                result = cursor.fetchone()
                if result:
                    try:
                        old_data = json.loads(result[1])
                        
                        # Create new BusinessEvent object with explicit format
                        new_business_event = self._create_business_event_from_old_data(old_data, child_id)
                        
                        # Store in new database
                        self._store_business_event(new_business_event)
                        
                        migrated_count += 1
                        print(f"✅ Migrated business event: {child_id}")
                        
                    except json.JSONDecodeError:
                        print(f"⚠️  Error parsing child object JSON for {child_id}")
                        continue
            
            print(f"✅ Successfully migrated {migrated_count} business events")
            return migrated_count
            
        except Exception as e:
            print(f"❌ Error during business event migration: {e}")
            return 0
    
    def _create_business_event_from_old_data(self, old_data: Dict[str, Any], event_id: str) -> Dict[str, Any]:
        """Create a new BusinessEvent object from old data format"""
        # Convert old field names to new format
        date_value = old_data.get('Date') or old_data.get('date')
        amount_value = old_data.get('Amount') or old_data.get('amount')
        category_value = old_data.get('Category') or old_data.get('category')
        description_value = old_data.get('Description') or old_data.get('description') or ''
        
        # Convert timestamp to date if needed
        if isinstance(date_value, (int, str)) and len(str(date_value)) == 10:
            try:
                timestamp = int(date_value)
                date_value = datetime.fromtimestamp(timestamp).strftime('%Y-%m-%d')
            except (ValueError, TypeError):
                date_value = datetime.now().strftime('%Y-%m-%d')
        
        # Convert "Ad cost" to "Ads cost"
        if category_value == "Ad cost":
            category_value = "Ads cost"
        
        # Create explicit new BusinessEvent object
        new_business_event = {
            'id': event_id,
            'date': date_value or datetime.now().strftime('%Y-%m-%d'),
            'amount': float(amount_value) if amount_value else 0.0,
            'category': category_value or 'Unknown',
            'description': description_value,
            'userId': 'seva',  # Explicit user ID as specified
            'createdAt': datetime.now().isoformat(),
            'updatedAt': datetime.now().isoformat()
        }
        
        return new_business_event
    
    def _store_business_event(self, business_event: Dict[str, Any]):
        """Store business event in new database"""
        cursor = self.new_conn.cursor()
        
        # Create path for new database structure
        path = f"/seva/business_events/{business_event['id']}"
        
        # Insert into new database
        cursor.execute("""
            INSERT OR REPLACE INTO documents (path, data, created_at, updated_at)
            VALUES (?, ?, CURRENT_TIMESTAMP, CURRENT_TIMESTAMP)
        """, (path, json.dumps(business_event)))
        
        self.new_conn.commit()
    
    def verify_migration(self) -> bool:
        """Verify that the migration was successful"""
        try:
            cursor = self.new_conn.cursor()
            
            # Check if business events were migrated
            cursor.execute("""
                SELECT COUNT(*) as count FROM documents 
                WHERE path LIKE '/seva/business_events/%'
            """)
            
            result = cursor.fetchone()
            count = result['count'] if result else 0
            
            print(f"📊 Verification: Found {count} business events in new database")
            
            if count > 0:
                # Show a sample of migrated data
                cursor.execute("""
                    SELECT path, data FROM documents 
                    WHERE path LIKE '/seva/business_events/%'
                    LIMIT 3
                """)
                
                samples = cursor.fetchall()
                print("\n📋 Sample migrated data:")
                for sample in samples:
                    try:
                        data = json.loads(sample['data'])
                        print(f"  - {sample['path']}: {data.get('description', data.get('id', 'No description'))}")
                    except:
                        print(f"  - {sample['path']}: [Data parsing error]")
            
            return count > 0
            
        except Exception as e:
            print(f"❌ Error during verification: {e}")
            return False
    
    def close_connections(self):
        """Close database connections"""
        if self.old_conn:
            self.old_conn.close()
        if self.new_conn:
            self.new_conn.close()
        print("🔒 Database connections closed")
    
    def run_migration(self) -> bool:
        """Run the complete migration process"""
        print("🚀 Starting database migration...")
        print(f"📁 Old database: {self.old_db_path}")
        print(f"📁 New database: {self.new_db_path}")
        print("-" * 50)
        
        try:
            # Step 1: Connect to databases
            self.connect_databases()
            
            # Step 2: Migrate business events
            migrated_count = self.migrate_business_events()
            
            if migrated_count == 0:
                print("⚠️  No business events were migrated")
                return False
            
            # Step 3: Verify migration
            success = self.verify_migration()
            
            print("-" * 50)
            if success:
                print("✅ Migration completed successfully!")
            else:
                print("❌ Migration completed with issues")
            
            return success
            
        except Exception as e:
            print(f"❌ Migration failed: {e}")
            return False
        finally:
            self.close_connections()


def main():
    parser = argparse.ArgumentParser(description='Migrate data from old Materia database to new v5 structure')
    parser.add_argument('old_db', help='Path to the old database file')
    parser.add_argument('new_db', help='Path to the new database file')
    parser.add_argument('--verbose', '-v', action='store_true', help='Enable verbose output')
    
    args = parser.parse_args()
    
    # Validate file paths
    if not os.path.exists(args.old_db):
        print(f"❌ Old database file not found: {args.old_db}")
        sys.exit(1)
    
    if not os.path.exists(args.new_db):
        print(f"❌ New database file not found: {args.new_db}")
        sys.exit(1)
    
    # Run migration
    migrator = DatabaseMigrator(args.old_db, args.new_db)
    success = migrator.run_migration()
    
    sys.exit(0 if success else 1)


if __name__ == "__main__":
    main()
