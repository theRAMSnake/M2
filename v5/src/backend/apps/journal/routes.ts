import { Router } from 'express';
import { JournalService } from './service';
import { AuthRequest, JournalFolderRequest, JournalPageRequest } from '../../types';
import { 
  handleServiceError, 
  generateId, 
  createValidationMiddleware, 
  sendSuccessResponse, 
  sendCreatedResponse,
  checkAuthorization 
} from '../../utils/routeHelpers';

const router = Router();
const journalService = new JournalService();

// Validation middleware for folder data
const validateFolderData = createValidationMiddleware(['name']);

// Validation middleware for page data
const validatePageData = createValidationMiddleware(['title', 'content']);

// GET /api/journal/items - Get all items in a specific path
router.get('/items', checkAuthorization, async (req: AuthRequest, res) => {
  try {
    const path = req.query.path as string || '';
    const items = await journalService.getItems(req.user!.id, path);
    sendSuccessResponse(res, { items });
  } catch (error) {
    handleServiceError(error, res, 'getting journal items');
  }
});

// GET /api/journal/tree - Get tree structure for lazy loading
router.get('/tree', checkAuthorization, async (req: AuthRequest, res) => {
  try {
    const path = req.query.path as string || '';
    const items = await journalService.getTreeStructure(req.user!.id, path);
    sendSuccessResponse(res, { items });
  } catch (error) {
    handleServiceError(error, res, 'getting journal tree structure');
  }
});

// GET /api/journal/folders/:id - Get a specific folder
router.get('/folders/:id', checkAuthorization, async (req: AuthRequest, res) => {
  try {
    const folderId = req.params.id;
    const folder = await journalService.getFolder(req.user!.id, folderId);
    
    if (!folder) {
      return res.status(404).json({ 
        error: 'Not found',
        message: 'Folder not found'
      });
    }

    sendSuccessResponse(res, folder);
  } catch (error) {
    handleServiceError(error, res, 'getting journal folder');
  }
});

// GET /api/journal/pages/:id - Get a specific page
router.get('/pages/:id', checkAuthorization, async (req: AuthRequest, res) => {
  try {
    const pageId = req.params.id;
    const page = await journalService.getPage(req.user!.id, pageId);
    
    if (!page) {
      return res.status(404).json({ 
        error: 'Not found',
        message: 'Page not found'
      });
    }

    sendSuccessResponse(res, page);
  } catch (error) {
    handleServiceError(error, res, 'getting journal page');
  }
});

// POST /api/journal/folders - Create a new folder
router.post('/folders', checkAuthorization, validateFolderData, async (req: AuthRequest, res) => {
  try {
    const folderData: JournalFolderRequest = req.body;
    const id = generateId(folderData.id);
    
    await journalService.createFolder(req.user!.id, {
      id,
      name: folderData.name,
      parentPath: folderData.parentPath,
    });

    sendCreatedResponse(res, { id }, 'Journal folder created successfully');
  } catch (error) {
    handleServiceError(error, res, 'creating journal folder');
  }
});

// POST /api/journal/pages - Create a new page
router.post('/pages', checkAuthorization, validatePageData, async (req: AuthRequest, res) => {
  try {
    const pageData: JournalPageRequest = req.body;
    const id = generateId(pageData.id);
    
    await journalService.createPage(req.user!.id, {
      id,
      title: pageData.title,
      content: pageData.content,
      parentPath: pageData.parentPath,
    });

    sendCreatedResponse(res, { id }, 'Journal page created successfully');
  } catch (error) {
    handleServiceError(error, res, 'creating journal page');
  }
});

// PUT /api/journal/folders/:id - Update a folder
router.put('/folders/:id', checkAuthorization, validateFolderData, async (req: AuthRequest, res) => {
  try {
    const folderId = req.params.id;
    const folderData: Partial<JournalFolderRequest> = req.body;

    await journalService.updateFolder(req.user!.id, folderId, folderData);
    sendSuccessResponse(res, undefined, 'Journal folder updated successfully');
  } catch (error) {
    handleServiceError(error, res, 'updating journal folder');
  }
});

// PUT /api/journal/pages/:id - Update a page
router.put('/pages/:id', checkAuthorization, validatePageData, async (req: AuthRequest, res) => {
  try {
    const pageId = req.params.id;
    const pageData: Partial<JournalPageRequest> = req.body;

    await journalService.updatePage(req.user!.id, pageId, pageData);
    sendSuccessResponse(res, undefined, 'Journal page updated successfully');
  } catch (error) {
    handleServiceError(error, res, 'updating journal page');
  }
});

// DELETE /api/journal/folders/:id - Delete a folder
router.delete('/folders/:id', checkAuthorization, async (req: AuthRequest, res) => {
  try {
    const folderId = req.params.id;
    
    // Check if folder exists
    const existingFolder = await journalService.getFolder(req.user!.id, folderId);
    if (!existingFolder) {
      return res.status(404).json({ 
        error: 'Not found',
        message: 'Folder not found'
      });
    }

    await journalService.deleteFolder(req.user!.id, folderId);
    sendSuccessResponse(res, undefined, 'Journal folder deleted successfully');
  } catch (error) {
    handleServiceError(error, res, 'deleting journal folder');
  }
});

// DELETE /api/journal/pages/:id - Delete a page
router.delete('/pages/:id', checkAuthorization, async (req: AuthRequest, res) => {
  try {
    const pageId = req.params.id;
    
    // Check if page exists
    const existingPage = await journalService.getPage(req.user!.id, pageId);
    if (!existingPage) {
      return res.status(404).json({ 
        error: 'Not found',
        message: 'Page not found'
      });
    }

    await journalService.deletePage(req.user!.id, pageId);
    sendSuccessResponse(res, undefined, 'Journal page deleted successfully');
  } catch (error) {
    handleServiceError(error, res, 'deleting journal page');
  }
});

// PUT /api/journal/move/:id - Move an item to a different parent
router.put('/move/:id', checkAuthorization, async (req: AuthRequest, res) => {
  try {
    const itemId = req.params.id;
    const { type, newParentPath } = req.body;

    if (!type || !['folder', 'page'].includes(type)) {
      return res.status(400).json({
        error: 'Bad Request',
        message: 'Type must be either "folder" or "page"'
      });
    }

    await journalService.moveItem(req.user!.id, itemId, type, newParentPath || '');
    sendSuccessResponse(res, undefined, 'Journal item moved successfully');
  } catch (error) {
    handleServiceError(error, res, 'moving journal item');
  }
});

export default router;
