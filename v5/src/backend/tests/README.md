# Backend Tests

This directory contains all backend tests for the Materia v5 application.

## Test Structure

```
tests/
├── database.test.ts          # DatabaseService tests
├── utils/
│   └── test-utils.ts         # Test utilities and helpers
├── index.ts                  # Test exports
└── README.md                 # This file
```

## Running Tests

### All Tests
```bash
npm test
```

### Specific Test File
```bash
npm test -- database.test.ts
```

### With Coverage
```bash
npm run test:coverage
```

### Watch Mode
```bash
npm run test:watch
```

## Test Utilities

### TestDatabaseService
A specialized version of DatabaseService for testing that:
- Uses temporary database files
- Automatically cleans up after tests
- Provides isolation between tests

```typescript
import { TestDatabaseService } from './utils/test-utils';

const db = new TestDatabaseService({
  dbPath: 'test-db.sqlite',
  cleanupAfterTest: true
});
```

### setupTestHierarchy()
Creates a standard test hierarchy for testing hierarchical operations:

```typescript
import { setupTestHierarchy } from './utils/test-utils';

beforeEach(async () => {
  await setupTestHierarchy(db);
});
```

### generateTestData()
Generates test data for batch operations:

```typescript
import { generateTestData } from './utils/test-utils';

const testData = generateTestData(10); // 10 test documents
```

## Writing New Tests

### 1. Create Test File
Create a new `.test.ts` file in the appropriate directory.

### 2. Import Test Utilities
```typescript
import { TestDatabaseService, setupTestHierarchy } from './utils/test-utils';
```

### 3. Use TestDatabaseService
```typescript
describe('MyFeature', () => {
  let db: TestDatabaseService;

  beforeEach(async () => {
    db = new TestDatabaseService({
      dbPath: `test-${Date.now()}.sqlite`
    });
  });

  afterEach(async () => {
    db.cleanup();
  });

  test('should do something', async () => {
    // Your test here
  });
});
```

### 4. Export from Index
Add your test to `index.ts`:
```typescript
export * from './my-feature.test';
```

## Test Best Practices

1. **Isolation**: Each test should be independent
2. **Cleanup**: Always clean up test data
3. **Descriptive Names**: Use clear test names
4. **Setup/Teardown**: Use beforeEach/afterEach for common setup
5. **Assertions**: Use specific assertions (toBe, toEqual, etc.)
6. **Error Testing**: Test both success and error cases

## Test Coverage

The test suite covers:
- ✅ Basic CRUD operations
- ✅ Hierarchical operations
- ✅ Search functionality
- ✅ Batch operations
- ✅ Error handling
- ✅ Edge cases
- ✅ Performance scenarios

## Adding New Test Utilities

1. Add utility functions to `utils/test-utils.ts`
2. Export them from the file
3. Import them in your test files
4. Document them in this README
