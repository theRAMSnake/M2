import { DatabaseService } from '../../storage/database';
import { JournalFolder, JournalPage, JournalItem, JournalFolderRequest, JournalPageRequest } from '../../types';

export class JournalService {
  private db: DatabaseService;

  constructor() {
    this.db = DatabaseService.getInstance();
  }

  // Get all items (folders and pages) in a specific hierarchical path
  async getItems(userId: string, path: string = ''): Promise<JournalItem[]> {
    try {
      const normalized = (path || '').toLowerCase();
      const basePath = `/${userId}/journal${normalized ? '/' + normalized : ''}`;
      const docs = await this.db.getChildren(basePath);

      const items: JournalItem[] = docs.map(doc => {
        const data = doc.data as any;
        const isPage = typeof data?.title === 'string';
        return {
          id: data.id,
          name: isPage ? data.title : data.name,
          type: isPage ? 'page' as const : 'folder' as const,
          path: (data.path || '').toLowerCase(),
          parentPath: normalized, // Use the requested path as parentPath for filtering
          createdBy: data.createdBy,
          createdAt: data.createdAt,
          updatedAt: data.updatedAt,
        };
      });

      return items.sort((a, b) => {
        if (a.type !== b.type) return a.type === 'folder' ? -1 : 1;
        return a.name.localeCompare(b.name);
      });
    } catch (error) {
      console.error(`Error getting journal items from path ${path}:`, error);
      return [];
    }
  }

  // Get a specific folder
  async getFolder(userId: string, folderId: string): Promise<(JournalFolder & { __path: string }) | null> {
    try {
      const found = await this.findById(userId, folderId);
      if (!found) return null;
      const data = found.data as any;
      if (data && data.name && !data.title) {
        return { ...(data as JournalFolder), __path: found.path };
      }
      return null;
    } catch (error) {
      console.error(`Error getting journal folder ${folderId}:`, error);
      return null;
    }
  }

  // Get a specific page
  async getPage(userId: string, pageId: string): Promise<(JournalPage & { __path: string }) | null> {
    try {
      const found = await this.findById(userId, pageId);
      if (!found) return null;
      const data = found.data as any;
      if (data && data.title) {
        return { ...(data as JournalPage), __path: found.path };
      }
      return null;
    } catch (error) {
      console.error(`Error getting journal page ${pageId}:`, error);
      return null;
    }
  }

  // Create a new folder
  async createFolder(userId: string, folderData: JournalFolderRequest): Promise<string> {
    const id = folderData.id || this.generateId();
    const parentPath = (folderData.parentPath || '').toLowerCase();
    const path = parentPath ? `${parentPath}/${folderData.name}` : folderData.name;
    
    const newFolder: JournalFolder = {
      id,
      name: folderData.name,
      path,
      parentPath,
      createdBy: userId,
      createdAt: new Date(),
      updatedAt: new Date(),
    };

    const fullDocPath = `/${userId}/journal${parentPath ? '/' + parentPath : ''}/${folderData.name}`.toLowerCase();
    newFolder.path = (newFolder.path || '').toLowerCase();
    await this.db.writeDocument(fullDocPath, newFolder);
    return id;
  }

  // Create a new page
  async createPage(userId: string, pageData: JournalPageRequest): Promise<string> {
    const id = pageData.id || this.generateId();
    const parentPath = (pageData.parentPath || '').toLowerCase();
    const path = parentPath ? `${parentPath}/${pageData.title}` : pageData.title;
    
    const newPage: JournalPage = {
      id,
      title: pageData.title,
      content: pageData.content || '',
      path,
      parentPath,
      createdBy: userId,
      createdAt: new Date(),
      updatedAt: new Date(),
    };

    const fullDocPath = `/${userId}/journal${parentPath ? '/' + parentPath : ''}/${pageData.title}`.toLowerCase();
    newPage.path = (newPage.path || '').toLowerCase();
    await this.db.writeDocument(fullDocPath, newPage);
    return id;
  }

  // Update a folder
  async updateFolder(userId: string, folderId: string, folderData: Partial<JournalFolderRequest>): Promise<void> {
    const existingFolder = await this.getFolder(userId, folderId);
    if (!existingFolder) {
      throw new Error('Folder not found');
    }

    const updatedFolder: JournalFolder = {
      ...existingFolder,
      name: folderData.name || existingFolder.name,
      updatedAt: new Date(),
    };

    const oldPath = (existingFolder as any).__path as string | undefined;
    if (folderData.name && folderData.name !== existingFolder.name) {
      updatedFolder.path = existingFolder.parentPath
        ? `${existingFolder.parentPath}/${folderData.name}`
        : folderData.name;
    }
    const newPath = `/${userId}/journal${updatedFolder.parentPath ? '/' + updatedFolder.parentPath : ''}/${updatedFolder.name}`.toLowerCase();
    await this.db.writeDocument(newPath, updatedFolder);
    if (oldPath && oldPath !== newPath) {
      await this.db.deleteDocument(oldPath);
    }
  }

  // Update a page
  async updatePage(userId: string, pageId: string, pageData: Partial<JournalPageRequest>): Promise<void> {
    const existingPage = await this.getPage(userId, pageId);
    if (!existingPage) {
      throw new Error('Page not found');
    }

    const updatedPage: JournalPage = {
      ...existingPage,
      title: pageData.title || existingPage.title,
      content: pageData.content !== undefined ? pageData.content : existingPage.content,
      updatedAt: new Date(),
    };

    const oldPath = (existingPage as any).__path as string | undefined;
    if (pageData.title && pageData.title !== existingPage.title) {
      updatedPage.path = existingPage.parentPath
        ? `${existingPage.parentPath}/${pageData.title}`
        : pageData.title;
    }
    const newPath = `/${userId}/journal${updatedPage.parentPath ? '/' + updatedPage.parentPath : ''}/${updatedPage.title}`.toLowerCase();
    await this.db.writeDocument(newPath, updatedPage);
    if (oldPath && oldPath !== newPath) {
      await this.db.deleteDocument(oldPath);
    }
  }

  // Delete a folder
  async deleteFolder(userId: string, folderId: string): Promise<void> {
    const folder = await this.getFolder(userId, folderId);
    if (!folder) {
      throw new Error('Folder not found');
    }

    // Check if folder has children
    const children = await this.getItems(userId, folder.path);
    if (children.length > 0) {
      throw new Error('Cannot delete folder that contains items');
    }

    await this.db.deleteDocument((folder as any).__path);
  }

  // Delete a page
  async deletePage(userId: string, pageId: string): Promise<void> {
    const page = await this.getPage(userId, pageId);
    if (!page) {
      throw new Error('Page not found');
    }

    await this.db.deleteDocument((page as any).__path);
  }

  // Move an item to a different parent
  async moveItem(userId: string, itemId: string, itemType: 'folder' | 'page', newParentPath: string): Promise<void> {
    if (itemType === 'folder') {
      const folder = await this.getFolder(userId, itemId);
      if (!folder) {
        throw new Error('Folder not found');
      }

      const newPath = newParentPath ? `${newParentPath}/${folder.name}` : folder.name;
      const updatedFolder: JournalFolder = {
        ...folder,
        path: newPath,
        parentPath: newParentPath,
        updatedAt: new Date(),
      };

      const oldPath = (folder as any).__path as string;
      const newFullPath = `/${userId}/journal${newParentPath ? '/' + newParentPath : ''}/${folder.name}`.toLowerCase();
      await this.db.writeDocument(newFullPath, updatedFolder);
      if (oldPath !== newFullPath) {
        await this.db.deleteDocument(oldPath);
      }
    } else {
      const page = await this.getPage(userId, itemId);
      if (!page) {
        throw new Error('Page not found');
      }

      const newPath = newParentPath ? `${newParentPath}/${page.title}` : page.title;
      const updatedPage: JournalPage = {
        ...page,
        path: newPath,
        parentPath: newParentPath,
        updatedAt: new Date(),
      };

      const oldPath = (page as any).__path as string;
      const newFullPath = `/${userId}/journal${newParentPath ? '/' + newParentPath : ''}/${page.title}`.toLowerCase();
      await this.db.writeDocument(newFullPath, updatedPage);
      if (oldPath !== newFullPath) {
        await this.db.deleteDocument(oldPath);
      }
    }
  }

  // Get the tree structure for lazy loading
  async getTreeStructure(userId: string, path: string = ''): Promise<JournalItem[]> {
    try {
      const normalized = (path || '').toLowerCase();
      const basePath = `/${userId}/journal${normalized ? '/' + normalized : ''}`;
      const docs = await this.db.getChildren(basePath);

      const items: JournalItem[] = docs.map(doc => {
        const data = doc.data as any;
        const isPage = typeof data?.title === 'string';
        return {
          id: data.id,
          name: isPage ? data.title : data.name,
          type: isPage ? 'page' as const : 'folder' as const,
          path: (data.path || '').toLowerCase(),
          parentPath: normalized, // Use the requested path as parentPath for filtering
          createdBy: data.createdBy,
          createdAt: data.createdAt,
          updatedAt: data.updatedAt,
        };
      });

      return items.sort((a, b) => {
        if (a.type !== b.type) return a.type === 'folder' ? -1 : 1;
        return a.name.localeCompare(b.name);
      });
    } catch (error) {
      console.error(`Error getting journal tree structure from path ${path}:`, error);
      return [];
    }
  }

  // Search for a journal document by id under this user's journal tree
  private async findById(userId: string, id: string): Promise<{ path: string, data: any } | null> {
    try {
      const results = await this.db.searchDocuments(id);
      for (const doc of results) {
        if (!doc.path.startsWith(`/${userId}/journal`)) continue;
        const data = doc.data as any;
        if (data?.id === id) {
          return { path: doc.path, data };
        }
      }
      return null;
    } catch (error) {
      console.error('Error finding document by id:', error);
      return null;
    }
  }

  // Private helper method to generate IDs
  private generateId(): string {
    return 'xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx'.replace(/[xy]/g, function(c) {
      const r = Math.random() * 16 | 0;
      const v = c === 'x' ? r : (r & 0x3 | 0x8);
      return v.toString(16);
    });
  }
}
