#!/usr/bin/env python3
"""
Migration script to transfer journal data from old Materia database to new v5 database structure.
Supports migrating journal_header and journal_content data with hierarchical relationships.
"""

import sqlite3
import json
import sys
import os
import argparse
from typing import List, Dict, Any, Optional, Tuple
from datetime import datetime
import uuid

class JournalMigrator:
    def __init__(self, old_db_path: str, new_db_path: str):
        self.old_db_path = old_db_path
        self.new_db_path = new_db_path
        self.old_conn = None
        self.new_conn = None
        self.user_id = 'snake'  # Default user ID
        
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
    
    def migrate_journal_data(self) -> int:
        """Migrate journal data from old database to new database"""
        migrated_count = 0
        
        try:
            cursor = self.old_conn.cursor()
            
            # Get all journal_header objects from journal_index table
            cursor.execute("SELECT Id, Json FROM journal_index")
            journal_headers = cursor.fetchall()
            
            print(f"🔍 Examining {len(journal_headers)} journal headers from journal_index table...")
            
            for obj_row in journal_headers:
                try:
                    obj_data = json.loads(obj_row[1])
                    obj_id = obj_row[0]
                    print(f"  ✅ Found journal header: {obj_id} - {obj_data.get('title', 'No title')}")
                except json.JSONDecodeError:
                    print(f"  ⚠️  JSON parse error for journal header: {obj_row[0]}")
                    continue
            
            
            if not journal_headers:
                print("⚠️  No journal_header objects found")
                return 0
            
            print(f"📊 Found {len(journal_headers)} journal headers")
            
            # Debug: Show sample journal headers
            if journal_headers:
                print("📋 Sample journal headers:")
                for header in journal_headers[:3]:
                    try:
                        data = json.loads(header[1])
                        print(f"  - ID: {header[0]}, Title: {data.get('title', 'N/A')}")
                    except:
                        print(f"  - ID: {header[0]} (JSON parse error)")
            
            # Get all connections for hierarchy mapping
            cursor.execute("SELECT Id, Json FROM connections")
            all_connections = cursor.fetchall()
            
            # Build connection maps
            hierarchy_map = {}  # parent_id -> [child_ids]
            extension_map = {}  # header_id -> content_id
            
            for conn_row in all_connections:
                try:
                    conn_data = json.loads(conn_row[1])
                    conn_type = conn_data.get('type')
                    a_id = conn_data.get('A')
                    b_id = conn_data.get('B')
                    
                    if conn_type == 'Hierarchy' and a_id and b_id:
                        if a_id not in hierarchy_map:
                            hierarchy_map[a_id] = []
                        hierarchy_map[a_id].append(b_id)
                    elif conn_type == 'Extension' and a_id and b_id:
                        extension_map[a_id] = b_id
                        
                except json.JSONDecodeError:
                    continue
            
            print(f"📊 Found {len(hierarchy_map)} hierarchy connections")
            print(f"📊 Found {len(extension_map)} extension connections")
            
            # Build the journal tree structure
            journal_tree = self._build_journal_tree(journal_headers, hierarchy_map, extension_map)
            
            # Migrate each journal item
            for item in journal_tree:
                migrated_count += self._migrate_journal_item(item)
            
            print(f"✅ Successfully migrated {migrated_count} journal items")
            return migrated_count
            
        except Exception as e:
            print(f"❌ Error during journal migration: {e}")
            return 0
    
    def _build_journal_tree(self, journal_headers: List, hierarchy_map: Dict, extension_map: Dict) -> List[Dict]:
        """Build a tree structure of journal items"""
        tree_items = []
        
        # First, create a map of all headers
        header_map = {}
        for header_row in journal_headers:
            try:
                header_data = json.loads(header_row[1])
                header_id = header_row[0]
                header_map[header_id] = {
                    'id': header_id,
                    'title': header_data.get('title', ''),
                    'type': 'folder',  # Default to folder
                    'content_id': extension_map.get(header_id),
                    'children': []
                }
            except json.JSONDecodeError:
                continue
        
        # Build hierarchy
        for parent_id, child_ids in hierarchy_map.items():
            if parent_id in header_map:
                for child_id in child_ids:
                    if child_id in header_map:
                        header_map[parent_id]['children'].append(header_map[child_id])
        
        # Find root items (items not in any hierarchy as children)
        all_child_ids = set()
        for child_ids in hierarchy_map.values():
            all_child_ids.update(child_ids)
        
        for header_id, item in header_map.items():
            if header_id not in all_child_ids:
                tree_items.append(item)
        
        return tree_items
    
    def _migrate_journal_item(self, item: Dict, parent_path: str = '') -> int:
        """Migrate a single journal item and its children"""
        migrated_count = 0
        
        try:
            # Determine if this is a page (has content) or folder
            is_page = item.get('content_id') is not None
            
            if is_page:
                # Get the content
                content = self._get_journal_content(item['content_id'])
                if content is None:
                    print(f"⚠️  Could not find content for journal item {item['id']}")
                    return 0
                
                # Create journal page
                new_page = self._create_journal_page(item, content, parent_path)
                self._store_journal_page(new_page)
                migrated_count += 1
                print(f"✅ Migrated journal page: {item['title']}")
            else:
                # Create journal folder
                new_folder = self._create_journal_folder(item, parent_path)
                self._store_journal_folder(new_folder)
                migrated_count += 1
                print(f"✅ Migrated journal folder: {item['title']}")
            
            # Migrate children
            for child in item.get('children', []):
                child_path = f"{parent_path}/{item['title']}" if parent_path else item['title']
                migrated_count += self._migrate_journal_item(child, child_path)
            
            return migrated_count
            
        except Exception as e:
            print(f"❌ Error migrating journal item {item.get('id', 'unknown')}: {e}")
            return 0
    
    def _get_journal_content(self, content_id: str) -> Optional[str]:
        """Get journal content by ID from journal_content_new table"""
        try:
            cursor = self.old_conn.cursor()
            cursor.execute("SELECT Json FROM journal_content_new WHERE Id = ?", (content_id,))
            result = cursor.fetchone()
            
            if result:
                content_data = json.loads(result[0])
                return content_data.get('content', '')
            
            return None
            
        except Exception as e:
            print(f"❌ Error getting journal content {content_id}: {e}")
            return None
    
    def _create_journal_folder(self, old_item: Dict, parent_path: str) -> Dict[str, Any]:
        """Create a new JournalFolder object from old data"""
        folder_path = f"{parent_path}/{old_item['title']}" if parent_path else old_item['title']
        
        new_folder = {
            'id': old_item['id'],
            'name': old_item['title'],
            'path': folder_path.lower(),
            'parentPath': parent_path.lower() if parent_path else '',
            'createdBy': self.user_id,
            'createdAt': datetime.now().isoformat(),
            'updatedAt': datetime.now().isoformat()
        }
        
        return new_folder
    
    def _create_journal_page(self, old_item: Dict, content: str, parent_path: str) -> Dict[str, Any]:
        """Create a new JournalPage object from old data"""
        page_path = f"{parent_path}/{old_item['title']}" if parent_path else old_item['title']
        
        new_page = {
            'id': old_item['id'],
            'title': old_item['title'],
            'content': content,
            'path': page_path.lower(),
            'parentPath': parent_path.lower() if parent_path else '',
            'createdBy': self.user_id,
            'createdAt': datetime.now().isoformat(),
            'updatedAt': datetime.now().isoformat()
        }
        
        return new_page
    
    def _store_journal_folder(self, folder: Dict[str, Any]):
        """Store journal folder in new database"""
        cursor = self.new_conn.cursor()
        
        # Create path for new database structure
        path = f"/{self.user_id}/journal/{folder['path']}"
        
        # Insert into new database
        cursor.execute("""
            INSERT OR REPLACE INTO documents (path, data, created_at, updated_at)
            VALUES (?, ?, CURRENT_TIMESTAMP, CURRENT_TIMESTAMP)
        """, (path, json.dumps(folder)))
        
        self.new_conn.commit()
    
    def _store_journal_page(self, page: Dict[str, Any]):
        """Store journal page in new database"""
        cursor = self.new_conn.cursor()
        
        # Create path for new database structure
        path = f"/{self.user_id}/journal/{page['path']}"
        
        # Insert into new database
        cursor.execute("""
            INSERT OR REPLACE INTO documents (path, data, created_at, updated_at)
            VALUES (?, ?, CURRENT_TIMESTAMP, CURRENT_TIMESTAMP)
        """, (path, json.dumps(page)))
        
        self.new_conn.commit()
    
    def verify_migration(self) -> bool:
        """Verify that the migration was successful"""
        try:
            cursor = self.new_conn.cursor()
            
            # Check if journal items were migrated
            cursor.execute("""
                SELECT COUNT(*) as count FROM documents 
                WHERE path LIKE '/snake/journal/%'
            """)
            
            result = cursor.fetchone()
            count = result['count'] if result else 0
            
            print(f"📊 Verification: Found {count} journal items in new database")
            
            if count > 0:
                # Show a sample of migrated data
                cursor.execute("""
                    SELECT path, data FROM documents 
                    WHERE path LIKE '/snake/journal/%'
                    LIMIT 5
                """)
                
                samples = cursor.fetchall()
                print("\n📋 Sample migrated data:")
                for sample in samples:
                    try:
                        data = json.loads(sample['data'])
                        item_type = 'page' if 'content' in data else 'folder'
                        name = data.get('title', data.get('name', 'Unknown'))
                        print(f"  - {sample['path']}: {item_type} '{name}'")
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
        print("🚀 Starting journal migration...")
        print(f"📁 Old database: {self.old_db_path}")
        print(f"📁 New database: {self.new_db_path}")
        print("-" * 50)
        
        try:
            # Step 1: Connect to databases
            self.connect_databases()
            
            # Step 2: Migrate journal data
            migrated_count = self.migrate_journal_data()
            
            if migrated_count == 0:
                print("⚠️  No journal items were migrated")
                return False
            
            # Step 3: Verify migration
            success = self.verify_migration()
            
            print("-" * 50)
            if success:
                print("✅ Journal migration completed successfully!")
            else:
                print("❌ Journal migration completed with issues")
            
            return success
            
        except Exception as e:
            print(f"❌ Migration failed: {e}")
            return False
        finally:
            self.close_connections()


def main():
    parser = argparse.ArgumentParser(description='Migrate journal data from old Materia database to new v5 structure')
    parser.add_argument('old_db', help='Path to the old database file')
    parser.add_argument('new_db', help='Path to the new database file')
    parser.add_argument('--user-id', default='snake', help='User ID for migrated data (default: snake)')
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
    migrator = JournalMigrator(args.old_db, args.new_db)
    migrator.user_id = args.user_id
    success = migrator.run_migration()
    
    sys.exit(0 if success else 1)


if __name__ == "__main__":
    main()
