# Database Migration Script

This folder contains the migration script to transfer data from the old Materia database to the new v5 database structure.

## Overview

The migration script (`migrate.py`) is designed to:
- Accept two parameters: `old_db` and `new_db`
- Query BusinessEvent data from all children of the 'finance' materia object in the old database
- Migrate this data to the new v5 database structure using the document storage API
- Support extensible migration for additional data types in the future

## How It Works

The script follows this process:
1. **Connects to both databases** - old Core format and new v5 format
2. **Finds the finance parent object** - looks for object with ID 'finance' in the objects table
3. **Discovers children** - uses the connections table to find all objects connected via 'Hierarchy' relationships
4. **Migrates data** - converts old format to new format and stores in the documents table
5. **Verifies migration** - checks that all data was successfully transferred

## Requirements

- Python 3.6 or higher
- SQLite3 (included with Python)
- Access to both old and new database files

## Usage

### Business Event Migration

```bash
python migrate.py /path/to/old/database.db /path/to/new/database.db
```

### Journal Migration

```bash
python migrate_journal.py /path/to/old/database.db /path/to/new/database.db
```

### Examples

```bash
# Migrate business events from old database to new v5 database
python migrate.py "C:\infra\materia.bu1755644404.375228" "C:\M2\v5\data\materia-v5.db"

# Migrate journal data from old database to new v5 database
python migrate_journal.py "C:\infra\materia.bu1755644404.375228" "C:\M2\v5\data\materia-v5.db"

# Or with relative paths
python migrate.py ../data/old-materia.db ./data/materia-v5.db
python migrate_journal.py ../data/old-materia.db ./data/materia-v5.db
```

### Command Line Options

- `old_db`: Path to the old Core format database file (e.g., backup from C:\infra)
- `new_db`: Path to the new v5 database file
- `--verbose` or `-v`: Enable verbose output (optional)

### Real-World Example

The script has been tested with real backup data and successfully migrated 177 business events from the old Core format to the new v5 structure.

## Supported Data Types

Currently supports migration of:

### BusinessEvent
- **Source**: All children of the 'finance' materia object in the old database
- **Destination**: `/seva/business_events/{event_id}` in the new database
- **Fields**: 
  - `id`: Unique identifier (auto-generated if missing)
  - `date`: Event date
  - `amount`: Financial amount (in euros)
  - `category`: Event category ('Client A', 'Client B', 'Ads cost', 'Other cost')
  - `description`: Optional description

### Journal Data
- **Source**: `journal_header` and `journal_content` objects with hierarchical relationships
- **Destination**: `/seva/journal/{path}` in the new database
- **Fields**:
  - `id`: Unique identifier from old system
  - `name`/`title`: Journal item name/title
  - `type`: 'folder' or 'page'
  - `path`: Hierarchical path structure
  - `parentPath`: Parent folder path
  - `content`: Rich text content (for pages only)
  - `createdBy`: User ID (default: 'seva')
  - `createdAt`/`updatedAt`: Timestamps

## Migration Process

1. **Connection**: Connects to both old and new databases
2. **Setup**: Creates required tables in the new database if they don't exist
3. **Discovery**: Searches for finance-related data in the old database
4. **Migration**: Transfers BusinessEvent data to the new structure
5. **Verification**: Confirms successful migration and shows sample data

## Database Structure

### Old Database
The script attempts to find finance data in various possible structures:
- `objects` table with path-based queries
- Any table with a `data` column containing finance-related JSON
- Fallback search across all tables for BusinessEvent patterns

### New Database
Uses the v5 document-based structure:
```sql
CREATE TABLE documents (
    id INTEGER PRIMARY KEY,
    path TEXT UNIQUE NOT NULL,
    data TEXT NOT NULL,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    updated_at DATETIME DEFAULT CURRENT_TIMESTAMP
);
```

## Error Handling

The script includes robust error handling:
- Validates file paths before starting
- Handles missing tables gracefully
- Continues migration even if individual records fail
- Provides detailed error messages and progress indicators
- Rolls back changes on critical errors

## Output

The script provides detailed output including:
- ✅ Success indicators
- 📊 Progress statistics
- ⚠️ Warnings for non-critical issues
- ❌ Error messages for failures
- 📋 Sample of migrated data for verification

## Extending the Migration

To add support for additional data types:

1. Add a new method to the `DatabaseMigrator` class (e.g., `migrate_journal_entries`)
2. Implement the data extraction logic for the old database
3. Define the new database path structure
4. Add the migration call to the `run_migration` method
5. Update this README with the new supported type

## Testing

### Test Journal Migration

Run the test script to verify journal migration functionality:

```bash
python test_journal_migration.py
```

This will:
- Create sample old database with journal data
- Run the migration process
- Verify the results
- Clean up temporary files

### Test Business Event Migration

Run the existing test script:

```bash
python test_migration.py
```

## Troubleshooting

### Common Issues

1. **"Old database file not found"**
   - Verify the path to the old database file
   - Ensure you have read permissions

2. **"No finance objects found"**
   - The old database may not contain finance data
   - Check if the database structure matches expected format

3. **"No business events were migrated"**
   - Data may not match the BusinessEvent format
   - Check the old database structure and data format

4. **"No journal_header objects found"**
   - The old database may not contain journal data
   - Check if the database has the expected journal structure

5. **"No journal items were migrated"**
   - Journal data may not match the expected format
   - Check the old database structure and connection relationships

### Debug Mode

Use the `--verbose` flag for additional debugging information:
```bash
python migrate.py old.db new.db --verbose
python migrate_journal.py old.db new.db --verbose
```

## Security Notes

- The script only reads from the old database
- Creates a new database or overwrites existing one
- No data is modified in the source database
- Always backup your data before running migrations

## License

This migration script is part of the M2 project and follows the same licensing terms.
