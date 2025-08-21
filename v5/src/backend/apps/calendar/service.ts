import { DatabaseService } from '../../storage/database';
import { CalendarItem } from '../../types';

export class CalendarService {
  private db: DatabaseService;

  constructor() {
    this.db = DatabaseService.getInstance();
  }

  async getCalendarItems(userId: string): Promise<CalendarItem[]> {
    try {
      // Get user's own calendar items
      const userPath = `/${userId}/calendar_items`;
      const userDocuments = await this.db.listDocuments(userPath);
      
      // Get public calendar items from all users (non-private)
      const publicItems: CalendarItem[] = [];
      try {
        // Check calendar items from both known users
        const otherUsers = ['snake', 'seva'].filter(user => user !== userId);
        
        for (const otherUser of otherUsers) {
          const otherUserPath = `/${otherUser}/calendar_items`;
          try {
            const otherUserDocuments = await this.db.listDocuments(otherUserPath);
            for (const doc of otherUserDocuments) {
              try {
                const itemData = await this.db.readDocument(doc.path);
                if (itemData && !itemData.isPrivate) {
                  // Handle legacy items without endTime - set endTime to 1 hour after start
                  if (!itemData.endTime && itemData.dateTime) {
                    const startTime = new Date(itemData.dateTime);
                    const endTime = new Date(startTime.getTime() + 60 * 60 * 1000); // 1 hour later
                    itemData.endTime = endTime.toISOString();
                  }
                  publicItems.push(itemData as CalendarItem);
                }
              } catch (error) {
                // Skip invalid documents
                console.error(`Error reading calendar document ${doc.path}:`, error);
              }
            }
          } catch (error) {
            // Skip users without calendar items
            console.error(`Error getting calendar items for user ${otherUser}:`, error);
          }
        }
      } catch (error) {
        console.error('Error getting public calendar items:', error);
      }
      
      // Get user's own items
      const userItems: CalendarItem[] = [];
      for (const doc of userDocuments) {
        try {
          const itemData = await this.db.readDocument(doc.path);
          if (itemData) {
            // Handle legacy items without endTime - set endTime to 1 hour after start
            if (!itemData.endTime && itemData.dateTime) {
              const startTime = new Date(itemData.dateTime);
              const endTime = new Date(startTime.getTime() + 60 * 60 * 1000); // 1 hour later
              itemData.endTime = endTime.toISOString();
            }
            userItems.push(itemData as CalendarItem);
          }
        } catch (error) {
          console.error(`Error reading calendar document ${doc.path}:`, error);
        }
      }
      
      // Combine and sort by dateTime
      const allItems = [...userItems, ...publicItems];
      return allItems.sort((a, b) => new Date(a.dateTime).getTime() - new Date(b.dateTime).getTime());
    } catch (error) {
      console.error('Error getting calendar items:', error);
      return [];
    }
  }

  async saveCalendarItem(userId: string, item: CalendarItem): Promise<void> {
    try {
      const path = `/${userId}/calendar_items/${item.id}`;
      const itemWithMetadata = {
        ...item,
        createdBy: userId,
        updatedAt: new Date(),
        // Preserve createdAt if it exists, otherwise set it
        createdAt: item.createdAt || new Date(),
      };
      await this.db.writeDocument(path, itemWithMetadata);
    } catch (error) {
      console.error('Error saving calendar item:', error);
      throw error;
    }
  }

  async deleteCalendarItem(userId: string, itemId: string): Promise<void> {
    try {
      const path = `/${userId}/calendar_items/${itemId}`;
      await this.db.deleteDocument(path);
    } catch (error) {
      console.error('Error deleting calendar item:', error);
      throw error;
    }
  }

  async addCalendarItem(userId: string, item: Omit<CalendarItem, 'createdBy' | 'createdAt' | 'updatedAt'>): Promise<void> {
    try {
      const newItem: CalendarItem = {
        ...item,
        createdBy: userId,
        createdAt: new Date(),
        updatedAt: new Date(),
      };
      await this.saveCalendarItem(userId, newItem);
    } catch (error) {
      console.error('Error adding calendar item:', error);
      throw error;
    }
  }

  async updateCalendarItem(userId: string, itemId: string, updatedItem: Omit<CalendarItem, 'createdBy' | 'createdAt' | 'updatedAt'>): Promise<void> {
    try {
      // First check if the item exists and belongs to the user
      const existingItem = await this.getCalendarItem(userId, itemId);
      if (!existingItem) {
        throw new Error('Calendar item not found');
      }
      
      if (existingItem.createdBy !== userId) {
        throw new Error('Not authorized to update this calendar item');
      }
      
      // Ensure the item ID matches and preserve metadata
      const itemToSave: CalendarItem = { 
        ...updatedItem, 
        id: itemId, 
        createdBy: existingItem.createdBy,
        createdAt: existingItem.createdAt,
        updatedAt: new Date(),
      };
      await this.saveCalendarItem(userId, itemToSave);
    } catch (error) {
      console.error('Error updating calendar item:', error);
      throw error;
    }
  }

  async getCalendarItem(userId: string, itemId: string): Promise<CalendarItem | null> {
    try {
      const path = `/${userId}/calendar_items/${itemId}`;
      const itemData = await this.db.readDocument(path);
      return itemData as CalendarItem || null;
    } catch (error) {
      console.error('Error getting calendar item:', error);
      return null;
    }
  }
}
